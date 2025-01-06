#include "optimizer/optimizer.h"

namespace bustub {

/**
 * @brief optimize sort + limit as top N
 */
auto Optimizer::OptimizeSortLimitAsTopN(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  // TODO(student): implement sort + limit -> top N optimizer rule
  return plan;
}

}  // namespace bustub
