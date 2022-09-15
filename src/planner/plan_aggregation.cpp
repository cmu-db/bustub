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
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/projection_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"
#include "type/type_id.h"

namespace bustub {

auto Planner::PlanAggCall(const BoundAggCall &agg_call, const std::vector<const AbstractPlanNode *> &children)
    -> std::tuple<AggregationType, std::unique_ptr<AbstractExpression>> {
  if (agg_call.args_.size() != 1) {
    throw NotImplementedException("only agg call of one arg is supported for now");
  }
  std::unique_ptr<AbstractExpression> expr = nullptr;
  {
    // Create a new context that doesn't allow aggregation calls.
    auto guard = NewContext();
    auto [_, ret] = PlanExpression(*agg_call.args_[0], children);
    expr = std::move(ret);
  }
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

auto Planner::PlanSelectAgg(const SelectStatement &statement, const AbstractPlanNode *child)
    -> std::unique_ptr<AbstractPlanNode> {
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

  // Create a new context which allows aggrecation call.
  auto guard = NewContext();
  ctx_.allow_aggregation_ = true;

  std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;

  // Plan group by expressions
  std::vector<const AbstractExpression *> group_by_exprs;
  for (const auto &expr : statement.group_by_) {
    auto [col_name, abstract_expr] = PlanExpression(*expr, {child});
    auto abstract_expr_ptr = SaveExpression(std::move(abstract_expr));
    group_by_exprs.push_back(abstract_expr_ptr);
    output_schema.emplace_back(std::make_pair(std::move(col_name), abstract_expr_ptr));
  }

  // Rewrite all agg call inside having.
  if (!statement.having_->IsInvalid()) {
    AddAggCallToContext(*statement.having_);
  }

  // Rewrite all agg call inside expression to a pseudo one.
  for (auto &item : statement.select_list_) {
    AddAggCallToContext(*item);
  }

  // Phase-1: plan an aggregation plan node out of all of the information we have.
  std::vector<const AbstractExpression *> input_exprs;
  std::vector<AggregationType> agg_types;
  auto agg_begin_idx = group_by_exprs.size();  // agg-calls will be after group-bys in the output of agg.

  int term_idx = 0;
  for (const auto &item : ctx_.aggregations_) {
    if (item->type_ != ExpressionType::AGG_CALL) {
      throw NotImplementedException("alias for agg call is not supported for now");
    }
    const auto &agg_call = dynamic_cast<const BoundAggCall &>(*item);
    auto [agg_type, expr] = PlanAggCall(agg_call, {child});
    auto abstract_expr = SaveExpression(std::move(expr));
    input_exprs.push_back(abstract_expr);
    agg_types.push_back(agg_type);
    output_schema.emplace_back(std::make_pair(fmt::format("agg#{}", term_idx), abstract_expr));
    // TODO(chi): correctly infer the type of the agg output.
    ctx_.expr_in_agg_.emplace_back(
        std::make_unique<ColumnValueExpression>(0, agg_begin_idx + term_idx, TypeId::INTEGER));
    term_idx += 1;
  }

  auto agg_output_schema = SaveSchema(MakeOutputSchema(output_schema));
  // Create the aggregation plan node for the first phase (finally!)
  std::unique_ptr<AbstractPlanNode> plan = std::make_unique<AggregationPlanNode>(
      agg_output_schema, child, nullptr, std::move(group_by_exprs), std::move(input_exprs), std::move(agg_types));

  // Phase-2: plan filter / projection to match the original select list

  // Create filter based on the having clause
  if (!statement.having_->IsInvalid()) {
    auto [_, expr] = PlanExpression(*statement.having_, {plan.get()});
    plan = std::make_unique<FilterPlanNode>(agg_output_schema, SaveExpression(std::move(expr)),
                                            SavePlanNode(std::move(plan)));
  }

  // Plan normal select
  std::vector<const AbstractExpression *> exprs;
  output_schema.clear();
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
