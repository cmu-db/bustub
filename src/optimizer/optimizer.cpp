#include "optimizer/optimizer.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

auto Optimizer::Optimize(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  auto p1 = OptimizeMergeProjection(plan);
  auto p2 = OptimizeMergeFilterNLJ(p1);
  auto p3 = OptimizeNLJAsIndexJoin(p2);
  auto p4 = OptimizeNLJAsHashJoin(p3);
  auto p5 = OptimizeOrderByAsIndexScan(p4);
  return p5;
}

}  // namespace bustub
