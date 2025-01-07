//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// sort_executor.cpp
//
// Identification: src/execution/sort_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/sort_executor.h"

namespace bustub {

/**
 * Construct a new SortExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The sort plan to be executed
 */
SortExecutor::SortExecutor(ExecutorContext *exec_ctx, const SortPlanNode *plan,
                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {}

/** Initialize the sort */
void SortExecutor::Init() { throw NotImplementedException("SortExecutor is not implemented"); }

/**
 * Yield the next tuple from the sort.
 * @param[out] tuple The next tuple produced by the sort
 * @param[out] rid The next tuple RID produced by the sort
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto SortExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
