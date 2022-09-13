#include "planner/planner.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_binary_op.h"
#include "binder/expressions/bound_column_ref.h"
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
#include "execution/expressions/comparison_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/mock_scan_plan.h"
#include "execution/plans/nested_loop_join_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "fmt/format.h"

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

auto Planner::PlanExpression(const BoundExpression &expr, const std::vector<const AbstractPlanNode *> &children)
    -> std::unique_ptr<AbstractExpression> {
  switch (expr.type_) {
    case ExpressionType::AGG_CALL:
      throw Exception("agg call should be handled by PlanSelect");
    case ExpressionType::COLUMN_REF: {
      const auto &column_ref_expr = dynamic_cast<const BoundColumnRef &>(expr);
      if (children.empty()) {
        throw Exception("column ref should have at least one child");
      }
      if (children.size() == 1) {
        // Projections, Filters, and other executors evaluating expressions with one single child will
        // use this branch.
        const auto &child = children[0];
        auto schema = child->OutputSchema();
        uint32_t col_idx = schema->GetColIdx(fmt::format("{}.{}", column_ref_expr.table_, column_ref_expr.col_));
        auto col_type = schema->GetColumn(col_idx).GetType();
        return std::make_unique<ColumnValueExpression>(0, col_idx, col_type);
      }
      if (children.size() == 2) {
        /**
         * Joins will use this branch to plan expressions.
         *
         * If an expression is for join condition, e.g.
         * SELECT * from test_1 inner join test_2 on test_1.colA = test_2.col2
         * The plan will be like:
         * ```
         * NestedLoopJoin condition={ ColumnRef 0.0=ColumnRef 1.1 }
         *   SeqScan colA, colB
         *   SeqScan col1, col2
         * ```
         * In `ColumnRef n.m`, when executor is using the expression, it picks from its
         * nth children's mth column to get the data.
         */

        const auto &left = children[0];
        const auto &right = children[1];
        auto left_schema = left->OutputSchema();
        auto right_schema = right->OutputSchema();
        auto col_name = fmt::format("{}.{}", column_ref_expr.table_, column_ref_expr.col_);
        auto col_idx_left = left_schema->TryGetColIdx(col_name);
        auto col_idx_right = right_schema->TryGetColIdx(col_name);
        if (col_idx_left && col_idx_right) {
          throw bustub::Exception(fmt::format("ambiguous column name {}", col_name));
        }
        if (col_idx_left) {
          auto col_type = left_schema->GetColumn(*col_idx_left).GetType();
          return std::make_unique<ColumnValueExpression>(0, *col_idx_left, col_type);
        }
        if (col_idx_right) {
          auto col_type = right_schema->GetColumn(*col_idx_right).GetType();
          return std::make_unique<ColumnValueExpression>(1, *col_idx_right, col_type);
        }
        throw bustub::Exception(fmt::format("column name {} not found", col_name));
      }
      UNREACHABLE("no executor with expression has more than 2 children for now");
    }
    case ExpressionType::BINARY_OP: {
      const auto &binary_op_expr = dynamic_cast<const BoundBinaryOp &>(expr);
      auto left = PlanExpression(*binary_op_expr.larg_, children);
      auto right = PlanExpression(*binary_op_expr.rarg_, children);
      const auto &op_name = binary_op_expr.op_name_;
      if (op_name == "=") {
        return std::make_unique<ComparisonExpression>(SaveExpression(std::move(left)), SaveExpression(std::move(right)),
                                                      ComparisonType::Equal);
      }
      throw Exception(fmt::format("binary op {} not supported in planner yet", op_name));
    }
    default:
      break;
  }
  throw Exception(fmt::format("expression type {} not supported in planner yet", expr.type_));
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
    auto expr = PlanExpression(*statement.where_, {plan.get()});
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
      auto abstract_expr = SaveExpression(PlanExpression(*agg_call.args_[0], {plan.get()}));
      input_exprs.push_back(abstract_expr);
      if (agg_call.func_name_ == "min") {
        agg_types.push_back(AggregationType::MinAggregate);
        // TODO(chi): deduce correct output schema
        output_schema.emplace_back(std::make_pair(
            "min", SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))));
      } else if (agg_call.func_name_ == "max") {
        agg_types.emplace_back(AggregationType::MaxAggregate);
        output_schema.emplace_back(std::make_pair(
            "max", SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))));
      } else if (agg_call.func_name_ == "sum") {
        agg_types.emplace_back(AggregationType::SumAggregate);
        output_schema.emplace_back(std::make_pair(
            "sum", SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))));
      } else if (agg_call.func_name_ == "count") {
        agg_types.emplace_back(AggregationType::CountAggregate);
        output_schema.emplace_back(std::make_pair(
            "count", SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))));
      } else {
        throw Exception(fmt::format("unsupported agg_call {}", agg_call.func_name_));
      }
      term_idx += 1;
    }
    return std::make_unique<AggregationPlanNode>(
        SaveSchema(MakeOutputSchema(output_schema)), SavePlanNode(std::move(plan)), nullptr,
        std::vector<const AbstractExpression *>{}, move(input_exprs), move(agg_types));
  }

  if (agg_call_cnt == 0) {
    // just select
    return plan;
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
          return std::make_unique<MockScanPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), 100);
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
          SaveExpression(std::make_unique<ConstantValueExpression>(Value(TypeId::BOOLEAN, static_cast<int8_t>(true)))));
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
      auto join_condition = PlanExpression(*join.condition_, {left.get(), right.get()});
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
