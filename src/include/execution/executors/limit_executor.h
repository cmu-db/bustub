//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limit_executor.h
//
// Identification: src/include/execution/executors/limit_executor.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
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
  LimitExecutor(ExecutorContext *exec_ctx, const LimitPlanNode *plan,
                std::unique_ptr<AbstractExecutor> &&child_executor);

  void Init() override;

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
