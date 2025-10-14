//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limit_executor.cpp
//
// Identification: src/execution/limit_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/limit_executor.h"
#include "common/macros.h"

namespace bustub {

/**
 * Construct a new LimitExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The limit plan to be executed
 * @param child_executor The child executor from which limited tuples are pulled
 */
LimitExecutor::LimitExecutor(ExecutorContext *exec_ctx, const LimitPlanNode *plan,
                             std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {
  UNIMPLEMENTED("TODO(P3): Add implementation.");
}

/** Initialize the limit */
void LimitExecutor::Init() { UNIMPLEMENTED("TODO(P3): Add implementation."); }

/**
 * Yield the next tuple batch from the limit.
 * @param[out] tuple_batch The next tuple batch produced by the limit
 * @param[out] rid_batch The next tuple RID batch produced by the limit
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto LimitExecutor::Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch,
                         size_t batch_size) -> bool {
  UNIMPLEMENTED("TODO(P3): Add implementation.");
}

}  // namespace bustub
