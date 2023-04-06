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

InitCheckExecutor::InitCheckExecutor(ExecutorContext *exec_ctx, AbstractPlanNodeRef plan,
                                     std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor{exec_ctx}, plan_{std::move(plan)}, child_executor_{std::move(child_executor)} {}

void InitCheckExecutor::Init() {
  if (!child_executor_) {
    return;
  }
  n_init_++;
  // Initialize the child executor
  child_executor_->Init();
}

auto InitCheckExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  // Emit the next tuple
  auto result = child_executor_->Next(tuple, rid);
  if (result) {
    n_next_++;
  }
  return result;
}

}  // namespace bustub
