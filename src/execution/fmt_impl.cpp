#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "fmt/format.h"

namespace bustub {

auto AbstractPlanNode::ChildrenToString(int indent) const -> std::string {
  if (children_.empty()) {
    return "";
  }
  std::vector<std::string> children_str;
  children_str.reserve(children_.size());
  std::string indent_str(indent, ' ');
  for (const auto &child : children_) {
    auto child_str = child->ToString();
    auto lines = StringUtil::Split(child_str, '\n');
    for (auto &line : lines) {
      children_str.push_back(fmt::format("{}{}", indent_str, line));
    }
  }
  return fmt::format("\n{}", fmt::join(children_str, "\n"));
}

auto AggregationPlanNode::HelperVecExprFmt(const std::vector<const AbstractExpression *> &exprs) const -> std::string {
  std::vector<std::string> joins;
  joins.reserve(exprs.size());
  for (const auto &expr : exprs) {
    joins.push_back(fmt::format("{}", *expr));
  }
  return fmt::format("{}", fmt::join(joins, ", "));
}

auto AggregationPlanNode::PlanNodeToString() const -> std::string {
  auto aggregates = HelperVecExprFmt(aggregates_);
  return fmt::format("Agg {{ types=[{}], aggregates=[{}] }}", fmt::join(agg_types_, ", "), aggregates);
}

}  // namespace bustub
