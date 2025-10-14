//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// init_check_executor.cpp
//
// Identification: src/execution/init_check_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/init_check_executor.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * Construct a new InitCheckExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The init check plan to be executed
 * @param child_executor The child executor from which init calls are counted
 */
InitCheckExecutor::InitCheckExecutor(ExecutorContext *exec_ctx, AbstractPlanNodeRef plan,
                                     std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor{exec_ctx}, plan_{std::move(plan)}, child_executor_{std::move(child_executor)} {}

/** Initialize the InitCheck */
void InitCheckExecutor::Init() {
  if (!child_executor_) {
    return;
  }
  n_init_++;
  // Initialize the child executor
  child_executor_->Init();
}

/**
 * Yield the next tuple batch from the child executor.
 * @param[out] tuple_batch The next tuple batch produced by the child executor
 * @param[out] rid_batch The next tuple RID batch produced by the child executor
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto InitCheckExecutor::Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch,
                             size_t batch_size) -> bool {
  tuple_batch->clear();
  rid_batch->clear();

  // Emit the next tuple
  auto result = child_executor_->Next(tuple_batch, rid_batch, batch_size);
  if (result) {
    n_next_++;
  }
  return result;
}

}  // namespace bustub
