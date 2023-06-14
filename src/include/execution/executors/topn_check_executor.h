//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_check_executor.h
//
// Identification: src/include/execution/executors/topn_check_executor.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>

#include "execution/executors/abstract_executor.h"
#include "execution/executors/topn_executor.h"
#include "execution/plans/topn_plan.h"

namespace bustub {

/**
 * TopNCheckExecutor checks the number of items in TopN executor container
 */
class TopNCheckExecutor : public AbstractExecutor {
 public:
  /**
   * Construct a new TopNCheckExecutor instance.
   * @param exec_ctx The executor context
   * @param plan The TopN plan to be executed
   * @param child_executor The TopN child executor
   */
  TopNCheckExecutor(ExecutorContext *exec_ctx, const TopNPlanNode *plan,
                    std::unique_ptr<AbstractExecutor> &&child_executor, TopNExecutor *topn_executor);

  /** Initialize the TopNCheck */
  void Init() override;

  /**
   * Yield the next tuple from the child executor.
   * @param[out] tuple The next tuple produced by the child executor
   * @param[out] rid The next tuple RID produced by the child executor
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the child executor */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); };

 private:
  /** TopNCheckExecutor returns `false` when child executor is exhausted */
  constexpr static const bool EXECUTOR_EXHAUSTED{false};

  /** The TopNPlanNode to be executed */
  const TopNPlanNode *plan_;

  std::size_t prev_{0};

  /** The child executor from which tuples are obtained */
  std::unique_ptr<AbstractExecutor> child_executor_;

  TopNExecutor *topn_executor_;
};

}  // namespace bustub
