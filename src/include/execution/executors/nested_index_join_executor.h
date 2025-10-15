//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nested_index_join_executor.h
//
// Identification: src/include/execution/executors/nested_index_join_executor.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/nested_index_join_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * NestedIndexJoinExecutor executes index join operations.
 */
class NestedIndexJoinExecutor : public AbstractExecutor {
 public:
  NestedIndexJoinExecutor(ExecutorContext *exec_ctx, const NestedIndexJoinPlanNode *plan,
                          std::unique_ptr<AbstractExecutor> &&child_executor);

  /** @return The output schema for the nested index join */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

  void Init() override;

  auto Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch, size_t batch_size)
      -> bool override;

 private:
  /** The nested index join plan node. */
  const NestedIndexJoinPlanNode *plan_;
};
}  // namespace bustub
