//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// delete_executor.h
//
// Identification: src/include/execution/executors/delete_executor.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/delete_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * DeletedExecutor executes a delete on a table.
 * Deleted values are always pulled from a child.
 */
class DeleteExecutor : public AbstractExecutor {
 public:
  /**
   * Construct a new DeleteExecutor instance.
   * @param exec_ctx The executor context
   * @param plan The delete plan to be executed
   * @param child_executor The child executor that feeds the delete
   */
  DeleteExecutor(ExecutorContext *exec_ctx, const DeletePlanNode *plan,
                 std::unique_ptr<AbstractExecutor> &&child_executor);

  /** Initialize the delete */
  void Init() override;

  /**
   * Yield the next tuple from the delete.
   * @param[out] tuple The next tuple produced by the update
   * @param[out] rid The next tuple RID produced by the update
   * @return `false` unconditionally (throw to indicate failure)
   *
   * NOTE: DeleteExecutor::Next() does not use the `tuple` out-parameter.
   * NOTE: DeleteExecutor::Next() does not use the `rid` out-parameter.
   */
  bool Next([[maybe_unused]] Tuple *tuple, RID *rid) override;

  /** @return The output schema for the delete */
  const Schema *GetOutputSchema() override { return plan_->OutputSchema(); };

 private:
  /** The delete plan node to be executed */
  const DeletePlanNode *plan_;
  /** The child executor from which RIDs for deleted tuples are pulled */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
