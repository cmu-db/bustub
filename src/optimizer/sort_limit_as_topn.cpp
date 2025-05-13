//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// sort_limit_as_topn.cpp
//
// Identification: src/optimizer/sort_limit_as_topn.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

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
