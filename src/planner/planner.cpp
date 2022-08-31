#include "planner/planner.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/seq_scan_plan.h"

namespace bustub {

void Planner::PlanQuery(const BoundStatement &statement) {
  switch (statement.type_) {
    case StatementType::SELECT_STATEMENT: {
      plan_ = PlanSelect(dynamic_cast<const SelectStatement &>(statement));
      return;
    }
    default:
      throw Exception("the statement is not supported in planner yet");
  }
}

unique_ptr<AbstractExpression> Planner::PlanExpression(const BoundExpression &expr, const AbstractPlanNode &child) {
  switch (expr.type_) {
    case ExpressionType::AGG_CALL:
      throw Exception("agg call should be handled by PlanSelect");
    case ExpressionType::COLUMN_REF: {
      auto column_ref_expr = dynamic_cast<const BoundColumnRef &>(expr);
      auto schema = child.OutputSchema();
      // TODO: differentiate multiple tables
      uint32_t col_idx = schema->GetColIdx(column_ref_expr.col_);
      auto col_type = schema->GetColumn(col_idx).GetType();
      return std::make_unique<ColumnValueExpression>(0, col_idx, col_type);
    }
    default:
      break;
  }
  throw Exception("not supported");
}

std::unique_ptr<Schema> MakeOutputSchema(const std::vector<std::pair<std::string, const AbstractExpression *>> &exprs) {
  std::vector<Column> cols;
  cols.reserve(exprs.size());
  for (const auto &input : exprs) {
    if (input.second->GetReturnType() != TypeId::VARCHAR) {
      cols.emplace_back(input.first, input.second->GetReturnType(), input.second);
    } else {
      cols.emplace_back(input.first, input.second->GetReturnType(), 128, input.second);
    }
  }
  return std::make_unique<Schema>(cols);
}

unique_ptr<AbstractPlanNode> Planner::PlanSelect(const SelectStatement &statement) {
  unique_ptr<AbstractPlanNode> plan = nullptr;

  switch (statement.table_->type_) {
    case TableReferenceType::EMPTY:
      throw Exception("select value is not supported in planner yet");
    default:
      plan = PlanTableRef(*statement.table_);
      break;
  }

  // TODO: plan where

  if (!statement.group_by_.empty()) {
    // plan group-by agg
    throw Exception("cannot plan group-by agg");
  } else {
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
        assert(agg_call.args_.size() == 1);
        auto abstract_expr = SaveExpression(PlanExpression(*agg_call.args_[0], *plan));
        input_exprs.push_back(abstract_expr);
        if (agg_call.func_name_ == "min") {
          agg_types.push_back(AggregationType::MinAggregate);
          // TODO: deduce correct output schema
          output_schema.push_back(
              {"min", SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))});
        } else if (agg_call.func_name_ == "max") {
          agg_types.push_back(AggregationType::MaxAggregate);
          output_schema.push_back(
              {"max", SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))});
        } else if (agg_call.func_name_ == "sum") {
          agg_types.push_back(AggregationType::SumAggregate);
          output_schema.push_back(
              {"sum", SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))});
        } else if (agg_call.func_name_ == "count") {
          agg_types.push_back(AggregationType::CountAggregate);
          output_schema.push_back(
              {"count", SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))});
        } else {
          throw Exception(fmt::format("unsupported agg_call {}", agg_call.func_name_));
        }
        term_idx += 1;
      }
      return std::make_unique<AggregationPlanNode>(
          SaveSchema(MakeOutputSchema(output_schema)), SavePlanNode(move(plan)), nullptr,
          std::vector<const AbstractExpression *>{}, move(input_exprs), move(agg_types));
    } else if (agg_call_cnt == 0) {
      // just select
      return plan;
    } else {
      throw Exception("invalid select list: agg calls appear with other columns");
    }
  }
}

unique_ptr<AbstractPlanNode> Planner::PlanTableRef(const BoundTableRef &table_ref) {
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
      assert(table);
      auto schema = SaveSchema(make_unique<Schema>(table->schema_));
      return make_unique<SeqScanPlanNode>(schema, nullptr, table->oid_);
    }
    default:
      break;
  }
  throw Exception("the table ref type is not supported in planner yet");
}

}  // namespace bustub