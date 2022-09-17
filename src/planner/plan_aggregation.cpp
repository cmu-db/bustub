#include <memory>
#include <utility>

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/statement/select_statement.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/projection_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"

namespace bustub {

auto Planner::PlanAggCall(const BoundAggCall &agg_call, const std::vector<const AbstractPlanNode *> &children)
    -> std::tuple<AggregationType, std::unique_ptr<AbstractExpression>> {
  if (agg_call.args_.size() != 1) {
    throw NotImplementedException("only agg call of one arg is supported for now");
  }
  auto [_, expr] = PlanExpression(*agg_call.args_[0], children);
  if (agg_call.func_name_ == "min") {
    return {AggregationType::MinAggregate, std::move(expr)};
  }
  if (agg_call.func_name_ == "max") {
    return {AggregationType::MaxAggregate, std::move(expr)};
  }
  if (agg_call.func_name_ == "sum") {
    return {AggregationType::SumAggregate, std::move(expr)};
  }
  if (agg_call.func_name_ == "count") {
    return {AggregationType::CountAggregate, std::move(expr)};
  }
  throw Exception(fmt::format("unsupported agg_call {}", agg_call.func_name_));
}

auto Planner::PlanAggregation(const SelectStatement &statement, const AbstractPlanNode *child)
    -> std::unique_ptr<AbstractPlanNode> {
  // Plan group by expressions
  std::vector<const AbstractExpression *> group_by_exprs;
  for (const auto &expr : statement.group_by_) {
    auto [_, abstract_expr] = PlanExpression(*expr, {child});
    group_by_exprs.push_back(SaveExpression(std::move(abstract_expr)));
  }

  // Plan having
  std::unique_ptr<AbstractExpression> having_expr = nullptr;
  if (!statement.having_->IsInvalid()) {
    throw NotImplementedException("planning having is not supported for now");
  }

  // Plan select list
  std::vector<const AbstractExpression *> input_exprs;
  std::vector<AggregationType> agg_types;
  std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;

  int term_idx = 0;
  for (const auto &item : statement.select_list_) {
    if (item->type_ != ExpressionType::AGG_CALL) {
      // TODO(chi): support select item from group by clause
      throw NotImplementedException("only agg call is supported in select list for now");
    }
    const auto &agg_call = dynamic_cast<const BoundAggCall &>(*item);
    auto [agg_type, abstract_expression] = PlanAggCall(agg_call, {child});
    agg_types.push_back(agg_type);
    input_exprs.push_back(SaveExpression(std::move(abstract_expression)));
    output_schema.emplace_back(
        std::make_pair(fmt::format("agg#{}", term_idx),
                       SaveExpression(std::make_unique<AggregateValueExpression>(false, term_idx, TypeId::INTEGER))));
    term_idx += 1;
  }

  return std::make_unique<AggregationPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), child, nullptr,
                                               std::move(group_by_exprs), move(input_exprs), move(agg_types));
}

}  // namespace bustub
