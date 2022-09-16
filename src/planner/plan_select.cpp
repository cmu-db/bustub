#include <memory>
#include <optional>
#include <utility>

#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/tokens.h"
#include "common/enums/statement_type.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/limit_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/sort_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"
#include "type/type_id.h"

namespace bustub {

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

  bool has_agg = false;
  for (const auto &item : statement.select_list_) {
    if (item->HasAggregation()) {
      has_agg = true;
      break;
    }
  }

  if (!statement.having_->IsInvalid() || !statement.group_by_.empty() || has_agg) {
    // Plan aggregation
    plan = PlanSelectAgg(statement, SavePlanNode(std::move(plan)));
  } else {
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
    plan = std::make_unique<ProjectionPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), std::move(exprs),
                                                SavePlanNode(std::move(plan)));
  }

  // Plan ORDER BY
  if (!statement.sort_.empty()) {
    std::vector<std::pair<OrderByType, const AbstractExpression *>> order_bys;
    for (const auto &order_by : statement.sort_) {
      auto [_, expr] = PlanExpression(*order_by->expr_, {plan.get()});
      auto abstract_expr = SaveExpression(std::move(expr));
      order_bys.emplace_back(std::make_pair(order_by->type_, abstract_expr));
    }
    auto saved_plan_node = SavePlanNode(std::move(plan));
    plan = std::make_unique<SortPlanNode>(saved_plan_node->OutputSchema(), saved_plan_node, std::move(order_bys));
  }

  // Plan LIMIT
  if (!statement.limit_count_->IsInvalid() || !statement.limit_offset_->IsInvalid()) {
    std::optional<size_t> offset = std::nullopt;
    std::optional<size_t> limit = std::nullopt;

    if (!statement.limit_count_->IsInvalid()) {
      if (statement.limit_count_->type_ == ExpressionType::CONSTANT) {
        const auto constant_expr = dynamic_cast<BoundConstant &>(*statement.limit_count_);
        const auto val = constant_expr.val_.CastAs(TypeId::INTEGER);
        if (constant_expr.val_.GetTypeId() == TypeId::INTEGER) {
          limit = std::make_optional(constant_expr.val_.GetAs<int32_t>());
        } else {
          throw NotImplementedException("LIMIT clause must be an integer constant.");
        }
      } else {
        throw NotImplementedException("LIMIT clause must be an integer constant.");
      }
    }

    if (!statement.limit_offset_->IsInvalid()) {
      if (statement.limit_offset_->type_ == ExpressionType::CONSTANT) {
        const auto constant_expr = dynamic_cast<BoundConstant &>(*statement.limit_offset_);
        const auto val = constant_expr.val_.CastAs(TypeId::INTEGER);
        if (constant_expr.val_.GetTypeId() == TypeId::INTEGER) {
          offset = std::make_optional(constant_expr.val_.GetAs<int32_t>());
        } else {
          throw NotImplementedException("OFFSET clause must be an integer constant.");
        }
      } else {
        throw NotImplementedException("OFFSET clause must be an integer constant.");
      }
    }

    if (offset != std::nullopt) {
      throw NotImplementedException("OFFSET clause is not supported yet.");
    }

    auto saved_plan_node = SavePlanNode(std::move(plan));
    plan = std::make_unique<LimitPlanNode>(saved_plan_node->OutputSchema(), saved_plan_node, *limit);
  }

  return plan;
}

}  // namespace bustub
