//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// filter_executor.h
//
// Identification: src/include/execution/executors/filter_executor.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
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
  FilterExecutor(ExecutorContext *exec_ctx, const FilterPlanNode *plan,
                 std::unique_ptr<AbstractExecutor> &&child_executor);

  void Init() override;

  auto Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch, size_t batch_size)
      -> bool override;

  /** @return The output schema for the filter plan */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

 private:
  /** The filter plan node to be executed */
  const FilterPlanNode *plan_;

  /** The child executor from which tuples are obtained */
  std::unique_ptr<AbstractExecutor> child_executor_;

  /** child tuple batch & child RID batch */
  std::vector<Tuple> child_tuples_{};
  std::vector<RID> child_rids_{};

  /** child tuple batch offset */
  size_t child_offset_ = 0;
};
}  // namespace bustub
