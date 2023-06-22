//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limit_executor.h
//
// Identification: src/include/execution/executors/limit_executor.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>

#include "execution/executors/abstract_executor.h"
#include "execution/plans/limit_plan.h"

namespace bustub {

/**
 * LimitExecutor limits the number of output tuples produced by a child operator.
 */
class LimitExecutor : public AbstractExecutor {
 public:
  /**
   * Construct a new LimitExecutor instance.
   * @param exec_ctx The executor context
   * @param plan The limit plan to be executed
   * @param child_executor The child executor from which limited tuples are pulled
   */
  LimitExecutor(ExecutorContext *exec_ctx, const LimitPlanNode *plan,
                std::unique_ptr<AbstractExecutor> &&child_executor);

  /** Initialize the limit */
  void Init() override;

  /**
   * Yield the next tuple from the limit.
   * @param[out] tuple The next tuple produced by the limit
   * @param[out] rid The next tuple RID produced by the limit
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the limit */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); };

 private:
  /** The limit plan node to be executed */
  const LimitPlanNode *plan_;

  /** The child executor from which tuples are obtained */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
