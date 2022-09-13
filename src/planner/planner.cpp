#include "planner/planner.h"
#include <memory>
#include <utility>
#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_binary_op.h"
#include "binder/statement/select_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "binder/table_ref/bound_cross_product_ref.h"
#include "binder/table_ref/bound_join_ref.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/mock_scan_plan.h"
#include "execution/plans/nested_loop_join_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "fmt/format.h"
#include "type/value_factory.h"

namespace bustub {

void Planner::PlanQuery(const BoundStatement &statement) {
  switch (statement.type_) {
    case StatementType::SELECT_STATEMENT: {
      plan_ = PlanSelect(dynamic_cast<const SelectStatement &>(statement));
      return;
    }
    default:
      throw Exception(fmt::format("the statement {} is not supported in planner yet", statement.type_));
  }
}

auto MakeOutputSchema(const std::vector<std::pair<std::string, const AbstractExpression *>> &exprs)
    -> std::unique_ptr<Schema> {
  std::vector<Column> cols;
  cols.reserve(exprs.size());
  for (const auto &input : exprs) {
    if (input.second->GetReturnType() != TypeId::VARCHAR) {
      cols.emplace_back(input.first, input.second->GetReturnType(), input.second);
    } else {
      cols.emplace_back(input.first, input.second->GetReturnType(), VARCHAR_DEFAULT_LENGTH, input.second);
    }
  }
  return std::make_unique<Schema>(cols);
}

auto Planner::PlanSelect(const SelectStatement &statement) -> std::unique_ptr<AbstractPlanNode> {
  std::unique_ptr<AbstractPlanNode> plan = nullptr;

  switch (statement.table_->type_) {
    case TableReferenceType::EMPTY:
      throw Exception("select value is not supported in planner yet");
    default:
      plan = PlanTableRef(*statement.table_);
      break;
  }

  if (!statement.where_->IsInvalid()) {
    auto schema = plan->OutputSchema();
    auto [_, expr] = PlanExpression(*statement.where_, {plan.get()});
    plan = std::make_unique<FilterPlanNode>(schema, SaveExpression(std::move(expr)), SavePlanNode(std::move(plan)));
  }

  if (!statement.group_by_.empty()) {
    // plan group-by agg
    throw Exception("cannot plan group-by agg");
  }

  size_t agg_call_cnt = 0;
  for (const auto &item : statement.select_list_) {
    if (item->type_ == ExpressionType::AGG_CALL) {
      agg_call_cnt += 1;
    }
  }
  if (agg_call_cnt == statement.select_list_.size()) {
    // plan simple agg
    std::vector<const AbstractExpression *> input_exprs;
    std::vector<AggregationType> agg_types;
    std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;

    int term_idx = 0;
    for (const auto &item : statement.select_list_) {
      const auto &agg_call = dynamic_cast<const BoundAggCall &>(*item);
      BUSTUB_ASSERT(agg_call.args_.size() == 1, "only agg call of one arg is supported for now");
      auto [_, expr] = PlanExpression(*agg_call.args_[0], {plan.get()});
      auto abstract_expr = SaveExpression(std::move(expr));
      input_exprs.push_back(abstract_expr);
      if (agg_call.func_name_ == "min") {
        agg_types.push_back(AggregationType::MinAggregate);
      } else if (agg_call.func_name_ == "max") {
        agg_types.emplace_back(AggregationType::MaxAggregate);
      } else if (agg_call.func_name_ == "sum") {
        agg_types.emplace_back(AggregationType::SumAggregate);
      } else if (agg_call.func_name_ == "count") {
        agg_types.emplace_back(AggregationType::CountAggregate);
      } else {
        throw Exception(fmt::format("unsupported agg_call {}", agg_call.func_name_));
      }

      output_schema.emplace_back(
          std::make_pair(fmt::format("agg#{}", term_idx),
                         SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))));
      term_idx += 1;
    }
    return std::make_unique<AggregationPlanNode>(
        SaveSchema(MakeOutputSchema(output_schema)), SavePlanNode(std::move(plan)), nullptr,
        std::vector<const AbstractExpression *>{}, move(input_exprs), move(agg_types));
  }

  if (agg_call_cnt == 0) {
    std::vector<const AbstractExpression *> exprs;
    std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;
    std::vector<const AbstractPlanNode *> children = {plan.get()};
    for (const auto &item : statement.select_list_) {
      auto [name, expr] = PlanExpression(*item, {plan.get()});
      auto abstract_expr = SaveExpression(std::move(expr));
      exprs.push_back(abstract_expr);
      output_schema.emplace_back(std::make_pair(name, abstract_expr));
    }
    return std::make_unique<ProjectionPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), std::move(exprs),
                                                SavePlanNode(std::move(plan)));
  }

  throw Exception("invalid select list: agg calls appear with other columns");
}

auto Planner::PlanTableRef(const BoundTableRef &table_ref) -> std::unique_ptr<AbstractPlanNode> {
  switch (table_ref.type_) {
    case TableReferenceType::BASE_TABLE: {
      // We always scan ALL columns of the table, and use projection executor to
      // remove some of them, therefore simplifying the planning process.

      // It is also possible to have a "virtual" logical projection node, and
      // we can merge it with table scan when optimizing. But for now, having
      // an optimizer or not remains undecided. So I'd prefer going with a new
      // ProjectionExecutor.

      const auto &base_table_ref = dynamic_cast<const BoundBaseTableRef &>(table_ref);
      auto table = catalog_.GetTable(base_table_ref.table_);
      BUSTUB_ASSERT(table, "table not found");
      const auto &schema = table->schema_;
      std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;

      size_t idx = 0;
      for (const auto &column : schema.GetColumns()) {
        output_schema.emplace_back(fmt::format("{}.{}", base_table_ref.table_, column.GetName()),
                                   SaveExpression(std::make_unique<ColumnValueExpression>(0, idx, column.GetType())));
        idx += 1;
      }

      if (StringUtil::StartsWith(table->name_, "__")) {
        // Plan as MockScanExecutor if it is a mock table.
        if (StringUtil::StartsWith(table->name_, "__mock")) {
          return std::make_unique<MockScanPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), 100, table->name_);
        }
      } else {
        throw bustub::Exception(fmt::format("unsupported internal table: {}", table->name_));
      }
      // Otherwise, plan as normal SeqScan.
      return std::make_unique<SeqScanPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), nullptr, table->oid_);
    }
    case TableReferenceType::CROSS_PRODUCT: {
      const auto &cross_product = dynamic_cast<const BoundCrossProductRef &>(table_ref);
      auto left = PlanTableRef(*cross_product.left_);
      auto right = PlanTableRef(*cross_product.right_);
      std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;
      size_t idx = 0;
      for (const auto &column : left->OutputSchema()->GetColumns()) {
        output_schema.emplace_back(column.GetName(),
                                   SaveExpression(std::make_unique<ColumnValueExpression>(0, idx, column.GetType())));
        idx += 1;
      }
      idx = 0;
      for (const auto &column : right->OutputSchema()->GetColumns()) {
        output_schema.emplace_back(column.GetName(),
                                   SaveExpression(std::make_unique<ColumnValueExpression>(1, idx, column.GetType())));
        idx += 1;
      }
      return std::make_unique<NestedLoopJoinPlanNode>(
          SaveSchema(MakeOutputSchema(output_schema)),
          std::vector{SavePlanNode(std::move(left)), SavePlanNode(std::move(right))},
          SaveExpression(std::make_unique<ConstantValueExpression>(ValueFactory::GetBooleanValue(true))));
    }
    case TableReferenceType::JOIN: {
      const auto &join = dynamic_cast<const BoundJoinRef &>(table_ref);
      auto left = PlanTableRef(*join.left_);
      auto right = PlanTableRef(*join.right_);
      std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;
      size_t idx = 0;
      for (const auto &column : left->OutputSchema()->GetColumns()) {
        output_schema.emplace_back(column.GetName(),
                                   SaveExpression(std::make_unique<ColumnValueExpression>(0, idx, column.GetType())));
        idx += 1;
      }
      idx = 0;
      for (const auto &column : right->OutputSchema()->GetColumns()) {
        output_schema.emplace_back(column.GetName(),
                                   SaveExpression(std::make_unique<ColumnValueExpression>(1, idx, column.GetType())));
        idx += 1;
      }
      auto nlj_output_schema = SaveSchema(MakeOutputSchema(output_schema));
      auto [_, join_condition] = PlanExpression(*join.condition_, {left.get(), right.get()});
      auto nlj_node = std::make_unique<NestedLoopJoinPlanNode>(
          nlj_output_schema, std::vector{SavePlanNode(std::move(left)), SavePlanNode(std::move(right))},
          SaveExpression(std::move(join_condition)));
      return nlj_node;
    }
    default:
      break;
  }
  throw Exception(fmt::format("the table ref type {} is not supported in planner yet", table_ref.type_));
}

}  // namespace bustub
