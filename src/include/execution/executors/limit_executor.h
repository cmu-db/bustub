//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limit_executor.h
//
// Identification: src/include/execution/executors/limit_executor.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>

#include "execution/executors/abstract_executor.h"
#include "execution/plans/limit_plan.h"

namespace bustub {
/**
 * LimitExecutor limits the number of output tuples with an optional offset.
 */
class LimitExecutor : public AbstractExecutor {
 public:
  /**
   * Creates a new limit executor.
   * @param exec_ctx the executor context
   * @param plan the limit plan to be executed
   * @param child_executor the child executor that produces tuple
   */
  LimitExecutor(ExecutorContext *exec_ctx, const LimitPlanNode *plan,
                std::unique_ptr<AbstractExecutor> &&child_executor);

  const Schema *GetOutputSchema() override { return plan_->OutputSchema(); };

  void Init() override;

  bool Next(Tuple *tuple, RID *rid) override;

 private:
  /** The limit plan node to be executed. */
  const LimitPlanNode *plan_;
  /** The child executor to obtain value from. */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
