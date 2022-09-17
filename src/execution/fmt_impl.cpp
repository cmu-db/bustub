#include <type_traits>
#include "execution/plans/sort_plan.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/projection_plan.h"

namespace bustub {

auto AbstractPlanNode::ChildrenToString(int indent) const -> std::string {
  if (children_.empty()) {
    return "";
  }
  std::vector<std::string> children_str;
  children_str.reserve(children_.size());
  auto indent_str = StringUtil::Indent(indent);
  for (const auto &child : children_) {
    auto child_str = child->ToString();
    auto lines = StringUtil::Split(child_str, '\n');
    for (auto &line : lines) {
      children_str.push_back(fmt::format("{}{}", indent_str, line));
    }
  }
  return fmt::format("\n{}", fmt::join(children_str, "\n"));
}

// TODO(chi): ALL BELOW HELPERS ARE NO LONGER NEED IF WE SWITCH TO std::unique_ptr INSTEAD OF RAW POINTERS!!!

static auto HelperVecExprFmt(const std::vector<const AbstractExpression *> &exprs) -> std::string {
  std::vector<std::string> joins;
  joins.reserve(exprs.size());
  for (const auto &expr : exprs) {
    joins.push_back(fmt::format("{}", *expr));
  }
  return fmt::format("{}", fmt::join(joins, ", "));
}

template <typename T>
static auto HelperVecPairExprFmt(const std::vector<std::pair<T, const AbstractExpression *>> &exprs) -> std::string {
  std::vector<std::string> joins;
  joins.reserve(exprs.size());
  for (const auto &[ty, expr] : exprs) {
    joins.push_back(fmt::format("({}, {})", ty, *expr));
  }
  return fmt::format("{}", fmt::join(joins, ", "));
}

auto AggregationPlanNode::PlanNodeToString() const -> std::string {
  auto aggregates = HelperVecExprFmt(aggregates_);
  return fmt::format("Agg {{ types={}, aggregates=[{}], having={}, group_by=[{}] }}", agg_types_, aggregates,
                     having_ == nullptr ? "" : having_->ToString(), HelperVecExprFmt(group_bys_));
}

auto ProjectionPlanNode::PlanNodeToString() const -> std::string {
  return fmt::format("Projection {{ exprs={} }}", HelperVecExprFmt(expressions_));
}

auto SortPlanNode::PlanNodeToString() const -> std::string {
  return fmt::format("Sort {{ order_bys={} }}", HelperVecPairExprFmt(order_bys_));
}

}  // namespace bustub
