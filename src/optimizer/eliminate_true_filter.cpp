#include <vector>
#include "execution/plans/filter_plan.h"
#include "execution/plans/limit_plan.h"
#include "execution/plans/sort_plan.h"
#include "execution/plans/topn_plan.h"

#include "optimizer/optimizer.h"

namespace bustub {

#ifdef BUSTUB_OPTIMIZER_HACK_REMOVE_AFTER_2022_FALL

auto Optimizer::OptimizeEliminateTrueFilter(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  std::vector<AbstractPlanNodeRef> children;
  for (const auto &child : plan->GetChildren()) {
    children.emplace_back(OptimizeEliminateTrueFilter(child));
  }

  auto optimized_plan = plan->CloneWithChildren(std::move(children));

  if (optimized_plan->GetType() == PlanType::Filter) {
    const auto &filter_plan = dynamic_cast<const FilterPlanNode &>(*optimized_plan);
    if (IsPredicateTrue(*filter_plan.GetPredicate())) {
      BUSTUB_ASSERT(optimized_plan->children_.size() == 1, "must have exactly one children");
      return optimized_plan->children_[0];
    }
  }

  return optimized_plan;
}

#endif

}  // namespace bustub
