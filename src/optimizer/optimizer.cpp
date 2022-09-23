#include "optimizer/optimizer.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

auto Optimizer::Optimize(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  auto p1 = OptimizeMergeProjection(plan);
  auto p2 = OptimizeMergeFilterNLJ(p1);
  auto p3 = OptimizeNLJAsHashJoin(p2);
  return p3;
}

}  // namespace bustub
