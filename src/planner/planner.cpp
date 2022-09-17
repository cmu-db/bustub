#include <memory>
#include <utility>

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "common/enums/statement_type.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/projection_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"

namespace bustub {

void Planner::PlanQuery(const BoundStatement &statement) {
  switch (statement.type_) {
    case StatementType::SELECT_STATEMENT: {
      plan_ = PlanSelect(dynamic_cast<const SelectStatement &>(statement));
      return;
    }
    case StatementType::INSERT_STATEMENT: {
      plan_ = PlanInsert(dynamic_cast<const InsertStatement &>(statement));
      return;
    }
    default:
      throw Exception(fmt::format("the statement {} is not supported in planner yet", statement.type_));
  }
}

auto Planner::MakeOutputSchema(const std::vector<std::pair<std::string, const AbstractExpression *>> &exprs)
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

  bool is_agg = false;

  if (!statement.group_by_.empty()) {
    is_agg = true;
  }

  if (!statement.having_->IsInvalid()) {
    is_agg = true;
  }

  for (const auto &item : statement.select_list_) {
    if (item->type_ == ExpressionType::AGG_CALL) {
      is_agg = true;
    }
  }

  if (is_agg) {
    // Plan aggregation
    return PlanAggregation(statement, SavePlanNode(std::move(plan)));
  }

  // Plan normal select
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

}  // namespace bustub
