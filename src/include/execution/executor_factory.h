//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// executor_factory.h
//
// Identification: src/include/execution/executor_factory.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>

#include "execution/executors/abstract_executor.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {
/**
 * ExecutorFactory creates executors for arbitrary plan nodes.
 */
class ExecutorFactory {
 public:
  static auto CreateExecutor(ExecutorContext *exec_ctx, const AbstractPlanNodeRef &plan)
      -> std::unique_ptr<AbstractExecutor>;
};
}  // namespace bustub
