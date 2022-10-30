//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// filter_executor.h
//
// Identification: src/include/execution/executors/filter_executor.h
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * The FilterExecutor executor executes a filter.
 */
class FilterExecutor : public AbstractExecutor {
 public:
  /**
   * Construct a new FilterExecutor instance.
   * @param exec_ctx The executor context
   * @param plan The filter plan to be executed
   * @param child_executor The child executor that feeds the filter
   */
  FilterExecutor(ExecutorContext *exec_ctx, const FilterPlanNode *plan,
                 std::unique_ptr<AbstractExecutor> &&child_executor);

  /** Initialize the filter */
  void Init() override;

  /**
   * Yield the next tuple from the filter.
   * @param[out] tuple The next tuple produced by the filter
   * @param[out] rid The next tuple RID produced by the filter
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the filter plan */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

 private:
  /** The filter plan node to be executed */
  const FilterPlanNode *plan_;

  /** The child executor from which tuples are obtained */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
