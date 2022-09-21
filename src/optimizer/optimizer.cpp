#include "optimizer/optimizer.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

auto Optimizer::Optimize(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  return OptimizeMergeProjection(plan);
}

}  // namespace bustub
