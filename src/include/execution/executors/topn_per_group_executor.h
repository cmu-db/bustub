//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_per_group_executor.h
//
// Identification: src/include/execution/executors/topn_per_group_executor.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/seq_scan_plan.h"
#include "execution/plans/topn_per_group_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * The TopNPerGroupExecutor executor executes a topn.
 */
class TopNPerGroupExecutor : public AbstractExecutor {
 public:
  TopNPerGroupExecutor(ExecutorContext *exec_ctx, const TopNPerGroupPlanNode *plan,
                       std::unique_ptr<AbstractExecutor> &&child_executor);

  void Init() override;
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

 private:
  /** The TopNPerGroup plan node to be executed */
  [[maybe_unused]] const TopNPerGroupPlanNode *plan_;
  /** The child executor from which tuples are obtained */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
