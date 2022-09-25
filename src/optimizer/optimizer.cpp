#include "optimizer/optimizer.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

auto Optimizer::Optimize(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  auto p1 = OptimizeMergeProjection(plan);
  auto p2 = OptimizeMergeFilterNLJ(p1);
  auto p3 = OptimizeNLJAsHashJoin(p2);
  auto p4 = OptimizeOrderByAsIndexScan(p3);
  return p4;
}

}  // namespace bustub
