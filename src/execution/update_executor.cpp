//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_executor.cpp
//
// Identification: src/execution/update_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>
#include "common/macros.h"

#include "execution/executors/update_executor.h"

namespace bustub {

/**
 * Construct a new UpdateExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The update plan to be executed
 * @param child_executor The child executor that feeds the update
 */
UpdateExecutor::UpdateExecutor(ExecutorContext *exec_ctx, const UpdatePlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {
  UNIMPLEMENTED("TODO(P3): Add implementation.");
}

/** Initialize the update */
void UpdateExecutor::Init() { UNIMPLEMENTED("TODO(P3): Add implementation."); }

/**
 * Yield the number of rows updated in the table.
 * @param[out] tuple_batch The tuple batch with one integer indicating the number of rows updated in the table
 * @param[out] rid_batch The next tuple RID batch produced by the update (ignore, not used)
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 *
 * NOTE: UpdateExecutor::Next() does not use the `rid_batch` out-parameter.
 * NOTE: UpdateExecutor::Next() returns true with the number of updated rows produced only once.
 */
auto UpdateExecutor::Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch,
                          size_t batch_size) -> bool {
  UNIMPLEMENTED("TODO(P3): Add implementation.");
}

}  // namespace bustub
