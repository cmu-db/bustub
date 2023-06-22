//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_executor.h
//
// Identification: src/include/execution/executors/update_executor.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/update_plan.h"
#include "storage/table/tuple.h"
#include "type/value_factory.h"

namespace bustub {

/**
 * UpdateExecutor executes an update on a table.
 * Updated values are always pulled from a child.
 */
class UpdateExecutor : public AbstractExecutor {
  friend class UpdatePlanNode;

 public:
  /**
   * Construct a new UpdateExecutor instance.
   * @param exec_ctx The executor context
   * @param plan The update plan to be executed
   * @param child_executor The child executor that feeds the update
   */
  UpdateExecutor(ExecutorContext *exec_ctx, const UpdatePlanNode *plan,
                 std::unique_ptr<AbstractExecutor> &&child_executor);

  /** Initialize the update */
  void Init() override;

  /**
   * Yield the next tuple from the update.
   * @param[out] tuple The next tuple produced by the update
   * @param[out] rid The next tuple RID produced by the update (ignore this)
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   *
   * NOTE: UpdateExecutor::Next() does not use the `rid` out-parameter.
   */
  auto Next([[maybe_unused]] Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the update */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

 private:
  /** The update plan node to be executed */
  const UpdatePlanNode *plan_;

  /** Metadata identifying the table that should be updated */
  const TableInfo *table_info_;

  /** The child executor to obtain value from */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
