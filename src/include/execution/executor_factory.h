//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// executor_factory.h
//
// Identification: src/include/execution/executor_factory.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
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
  /**
   * Creates a new executor given the executor context and plan node.
   * @param exec_ctx The executor context for the created executor
   * @param plan The plan node that needs to be executed
   * @return An executor for the given plan in the provided context
   */
  static std::unique_ptr<AbstractExecutor> CreateExecutor(ExecutorContext *exec_ctx, const AbstractPlanNode *plan);
};
}  // namespace bustub
