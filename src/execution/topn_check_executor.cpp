//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_check_executor.cpp
//
// Identification: src/execution/topn_check_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/topn_check_executor.h"
#include "execution/executors/topn_executor.h"

namespace bustub {

TopNCheckExecutor::TopNCheckExecutor(ExecutorContext *exec_ctx, const TopNPlanNode *plan,
                                     std::unique_ptr<AbstractExecutor> &&child_executor, TopNExecutor *topn_executor)
    : AbstractExecutor{exec_ctx},
      plan_(plan),
      child_executor_{std::move(child_executor)},
      topn_executor_(topn_executor) {}

void TopNCheckExecutor::Init() {
  if (!child_executor_) {
    return;
  }
  prev_ = 0;
  // Initialize the child executor
  child_executor_->Init();
}

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
