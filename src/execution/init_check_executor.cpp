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
 * Yield the next tuple from the child executor.
 * @param[out] tuple The next tuple produced by the child executor
 * @param[out] rid The next tuple RID produced by the child executor
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto InitCheckExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  // Emit the next tuple
  auto result = child_executor_->Next(tuple, rid);
  if (result) {
    n_next_++;
  }
  return result;
}

}  // namespace bustub
