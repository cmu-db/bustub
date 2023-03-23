//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// init_check_executor.cpp
//
// Identification: src/execution/init_check_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/init_check_executor.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

InitCheckExecutor::InitCheckExecutor(ExecutorContext *exec_ctx, const AbstractPlanNodeRef &plan,
                                     std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor{exec_ctx}, plan_{plan}, child_executor_{std::move(child_executor)} {}

void InitCheckExecutor::Init() {
  if (!child_executor_) {
    return;
  }
  n_init_++;
  // Initialize the child executor
  child_executor_->Init();
}

auto InitCheckExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  if (!child_executor_) {
    return EXECUTOR_EXHAUSTED;
  }

  // Emit the next tuple
  n_next_++;
  return child_executor_->Next(tuple, rid);
}

}  // namespace bustub
