//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// plan_aggregation.cpp
//
// Identification: src/planner/plan_aggregation.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>
#include <utility>

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/statement/select_statement.h"
#include "binder/tokens.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/projection_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"
#include "type/type_id.h"
#include "type/value_factory.h"

namespace bustub {

auto Planner::PlanAggCall(const BoundAggCall &agg_call, const std::vector<AbstractPlanNodeRef> &children)
    -> std::tuple<AggregationType, std::vector<AbstractExpressionRef>> {
  if (agg_call.is_distinct_) {
    throw NotImplementedException("distinct agg is not implemented yet");
  }

  std::vector<AbstractExpressionRef> exprs;

  {
    // Create a new context that doesn't allow aggregation calls.
    auto guard = NewContext();
    for (const auto &arg : agg_call.args_) {
      auto [_, ret] = PlanExpression(*arg, children);
      exprs.emplace_back(std::move(ret));
    }
  }

  return GetAggCallFromFactory(agg_call.func_name_, std::move(exprs));
}

// TODO(chi): clang-tidy on macOS will suggest changing it to const reference. Looks like a bug.

/* NOLINTNEXTLINE */
auto Planner::PlanSelectAgg(const SelectStatement &statement, AbstractPlanNodeRef child) -> AbstractPlanNodeRef {
  /* Transforming hash agg is complex. Let's see a concrete example here.
   *
   * Now that we have,
   * ```
   * select v3, max(v1) + max(v2) from table where <cond> group by v3 having count(v4) > count(v5);
   * ```
   *
   * Before this comment section, we have `from table where <cond>` planned as filter / table scan.
   *
   * Given this is a group-by query, we will have something like this in the final plan,
   * ```
   * <Parent Plan Nodes>
   *   Projection v3, max(v1) + max(v2)
   *     Filter count(v4) > count(v5)
   *       Aggregation group_by=v3 agg_types=[max, max, count, count] agg_expr=[v1, v2, v4, v5]
   *         <Filter / Table Scan> --- "table scan child"
   * ```
   *
   * For every expression in select list and having clause, we will do a two-phase planning. Firstly,
   * we plan all aggregation calls, and then we plan other expressions. Let's take `max(v1) + max(v2)` as
   * an example.
   *
   * - We plan `max(v1)` and `max(v2)` in the select list and `count(v4)`, `count(v5)` using the
   *   "table scan child". We get tuples of `[agg_type, abstract_expression]`.
   * - Then we construct the aggregation plan node with all required aggregations plus group-by expressions.
   * - After that we plan the filter `count(v4) > count(v5)` and the projection `v3`, `max(v1) + max(v2)`.
   * - That's all!
   */

  // Create a new context which allows aggregation call.
  auto guard = NewContext();
  ctx_.allow_aggregation_ = true;

  // Plan group by expressions
  std::vector<AbstractExpressionRef> group_by_exprs;
  std::vector<std::string> output_col_names;

  for (const auto &expr : statement.group_by_) {
    auto [col_name, abstract_expr] = PlanExpression(*expr, {child});
    group_by_exprs.emplace_back(std::move(abstract_expr));
    output_col_names.emplace_back(std::move(col_name));
  }

  // Rewrite all agg call inside having.
  if (!statement.having_->IsInvalid()) {
    AddAggCallToContext(*statement.having_);
  }

  // Rewrite all agg call inside expression to a pseudo one.
  // It replaces the agg call in select_list_ with a pseudo one with index
  // adds the real agg call to context.
  for (auto &item : statement.select_list_) {
    AddAggCallToContext(*item);
  }

  // Phase-1: plan an aggregation plan node out of all of the information we have.
  std::vector<AbstractExpressionRef> input_exprs;
  std::vector<AggregationType> agg_types;
  auto agg_begin_idx = group_by_exprs.size();  // agg-calls will be after group-bys in the output of agg.

  size_t term_idx = 0;
  for (const auto &item : ctx_.aggregations_) {
    if (item->type_ != ExpressionType::AGG_CALL) {
      throw NotImplementedException("alias for agg call is not supported for now");
    }
    const auto &agg_call = dynamic_cast<const BoundAggCall &>(*item);
    auto [agg_type, exprs] = PlanAggCall(agg_call, {child});
    if (exprs.size() > 1) {
      throw bustub::NotImplementedException("only agg call of zero/one arg is supported");
    }
    if (exprs.empty()) {
      // Rewrite count(*) into count(1)
      input_exprs.emplace_back(std::make_shared<ConstantValueExpression>(ValueFactory::GetIntegerValue(1)));
    } else {
      input_exprs.emplace_back(std::move(exprs[0]));
    }

    agg_types.push_back(agg_type);
    output_col_names.emplace_back(fmt::format("agg#{}", term_idx));
    ctx_.expr_in_agg_.emplace_back(
        std::make_unique<ColumnValueExpression>(0, agg_begin_idx + term_idx, Column("<agg_result>", TypeId::INTEGER)));

    term_idx += 1;
  }

  auto agg_output_schema = AggregationPlanNode::InferAggSchema(group_by_exprs, input_exprs, agg_types);

  // Create the aggregation plan node for the first phase (finally!)
  AbstractPlanNodeRef plan = std::make_shared<AggregationPlanNode>(
      std::make_shared<Schema>(ProjectionPlanNode::RenameSchema(agg_output_schema, output_col_names)), std::move(child),
      std::move(group_by_exprs), std::move(input_exprs), std::move(agg_types));

  // Phase-2: plan filter / projection to match the original select list

  // Create filter based on the having clause
  if (!statement.having_->IsInvalid()) {
    auto [_, expr] = PlanExpression(*statement.having_, {plan});
    plan = std::make_shared<FilterPlanNode>(std::make_shared<Schema>(plan->OutputSchema()), std::move(expr),
                                            std::move(plan));
  }

  // Plan normal select (within aggregation context)
  std::vector<AbstractExpressionRef> exprs;
  std::vector<std::string> final_output_col_names;
  std::vector<AbstractPlanNodeRef> children = {plan};
  for (const auto &item : statement.select_list_) {
    auto [name, expr] = PlanExpression(*item, {plan});
    exprs.push_back(std::move(expr));
    final_output_col_names.emplace_back(std::move(name));
  }

  return std::make_shared<ProjectionPlanNode>(
      std::make_shared<Schema>(
          ProjectionPlanNode::RenameSchema(ProjectionPlanNode::InferProjectionSchema(exprs), final_output_col_names)),
      std::move(exprs), std::move(plan));
}

}  // namespace bustub
