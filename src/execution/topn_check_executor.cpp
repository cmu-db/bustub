//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_check_executor.cpp
//
// Identification: src/execution/topn_check_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/topn_check_executor.h"
#include "execution/executors/topn_executor.h"

namespace bustub {

/**
 * Construct a new TopNCheckExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The TopN plan to be executed
 * @param child_executor The TopN child executor
 */
TopNCheckExecutor::TopNCheckExecutor(ExecutorContext *exec_ctx, const TopNPlanNode *plan,
                                     std::unique_ptr<AbstractExecutor> &&child_executor, TopNExecutor *topn_executor)
    : AbstractExecutor{exec_ctx},
      plan_(plan),
      child_executor_{std::move(child_executor)},
      topn_executor_(topn_executor) {}

/** Initialize the TopNCheck */
void TopNCheckExecutor::Init() {
  if (!child_executor_) {
    return;
  }
  prev_ = 0;
  // Initialize the child executor
  child_executor_->Init();
}

/**
 * Yield the next tuple from the child executor.
 * @param[out] tuple The next tuple produced by the child executor
 * @param[out] rid The next tuple RID produced by the child executor
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto TopNCheckExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  if (!child_executor_) {
    return EXECUTOR_EXHAUSTED;
  }

  BUSTUB_ASSERT(topn_executor_->GetNumInHeap() <= plan_->GetN(), "Cannot store more than N elements");
  if (prev_ > 0 && prev_ < plan_->GetN()) {
    BUSTUB_ASSERT(topn_executor_->GetNumInHeap() - prev_ == 1, "Did you implement GetNumInHeap() properly?");
  }
  prev_ = topn_executor_->GetNumInHeap();
  // Emit the next tuple
  return child_executor_->Next(tuple, rid);
}

}  // namespace bustub
