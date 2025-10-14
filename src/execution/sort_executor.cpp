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
 * Yield the next tuple batch from the sort.
 * @param[out] tuple_batch The next tuple batch produced by the sort
 * @param[out] rid_batch The next tuple RID batch produced by the sort
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto SortExecutor::Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch, size_t batch_size)
    -> bool {
  return false;
}

}  // namespace bustub
