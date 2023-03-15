#include <type_traits>
#include "execution/plans/update_plan.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/hash_join_plan.h"
#include "execution/plans/limit_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/sort_plan.h"
#include "execution/plans/topn_plan.h"

namespace bustub {

auto AbstractPlanNode::ChildrenToString(int indent, bool with_schema) const -> std::string {
  if (children_.empty()) {
    return "";
  }
  std::vector<std::string> children_str;
  children_str.reserve(children_.size());
  auto indent_str = StringUtil::Indent(indent);
  for (const auto &child : children_) {
    auto child_str = child->ToString(with_schema);
    auto lines = StringUtil::Split(child_str, '\n');
    for (auto &line : lines) {
      children_str.push_back(fmt::format("{}{}", indent_str, line));
    }
  }
  return fmt::format("\n{}", fmt::join(children_str, "\n"));
}

auto AggregationPlanNode::PlanNodeToString() const -> std::string {
  return fmt::format("Agg {{ types={}, aggregates={}, group_by={} }}", agg_types_, aggregates_, group_bys_);
}

auto HashJoinPlanNode::PlanNodeToString() const -> std::string {
  return fmt::format("HashJoin {{ type={}, left_key={}, right_key={} }}", join_type_, left_key_expressions_,
                     right_key_expressions_);
}

auto ProjectionPlanNode::PlanNodeToString() const -> std::string {
  return fmt::format("Projection {{ exprs={} }}", expressions_);
}

auto UpdatePlanNode::PlanNodeToString() const -> std::string {
  return fmt::format("Update {{ table_oid={}, target_exprs={} }}", table_oid_, target_expressions_);
}

auto SortPlanNode::PlanNodeToString() const -> std::string {
  return fmt::format("Sort {{ order_bys={} }}", order_bys_);
}

auto LimitPlanNode::PlanNodeToString() const -> std::string { return fmt::format("Limit {{ limit={} }}", limit_); }

auto TopNPlanNode::PlanNodeToString() const -> std::string {
  return fmt::format("TopN {{ n={}, order_bys={}}}", n_, order_bys_);
}

}  // namespace bustub
