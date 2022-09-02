#include "planner/planner.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/statement/select_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
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

auto Planner::PlanExpression(const BoundExpression &expr, const AbstractPlanNode &child)
    -> std::unique_ptr<AbstractExpression> {
  switch (expr.type_) {
    case ExpressionType::AGG_CALL:
      throw Exception("agg call should be handled by PlanSelect");
    case ExpressionType::COLUMN_REF: {
      auto column_ref_expr = dynamic_cast<const BoundColumnRef &>(expr);
      auto schema = child.OutputSchema();
      // TODO(chi): differentiate multiple tables
      uint32_t col_idx = schema->GetColIdx(column_ref_expr.col_);
      auto col_type = schema->GetColumn(col_idx).GetType();
      return std::make_unique<ColumnValueExpression>(0, col_idx, col_type);
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

  // TODO(chi): plan where

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
      auto abstract_expr = SaveExpression(PlanExpression(*agg_call.args_[0], *plan));
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
    return std::make_unique<AggregationPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), SavePlanNode(move(plan)),
                                                 nullptr, std::vector<const AbstractExpression *>{}, move(input_exprs),
                                                 move(agg_types));
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
        output_schema.emplace_back(column.GetName(),
                                   SaveExpression(std::make_unique<ColumnValueExpression>(0, idx, column.GetType())));
        idx += 1;
      }
      return std::make_unique<SeqScanPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), nullptr, table->oid_);
    }
    default:
      break;
  }
  throw Exception(fmt::format("the table ref type {} is not supported in planner yet", table_ref.type_));
}

}  // namespace bustub
