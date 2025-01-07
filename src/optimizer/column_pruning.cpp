//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// column_pruning.cpp
//
// Identification: src/optimizer/column_pruning.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "optimizer/optimizer.h"

namespace bustub {

/**
 * @brief column pruning for child plan following a projection plan
 * @param plan the plan to optimize
 * @return the new plan with column pruning
 * @note You may use this function to implement column pruning optimization.
 */
auto Optimizer::OptimizeColumnPruning(const bustub::AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  // Your code here
  return plan;
}

}  // namespace bustub
