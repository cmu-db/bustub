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
 * Delete executes a delete from a table.
 * Deleted tuple info come from a child executor.
 */
class DeleteExecutor : public AbstractExecutor {
 public:
  /**
   * Creates a new delete executor.
   * @param exec_ctx the executor context
   * @param plan the delete plan to be executed
   * @param child_executor the child executor (either sequential scan or index scan) to obtain tuple info
   */
  DeleteExecutor(ExecutorContext *exec_ctx, const DeletePlanNode *plan,
                 std::unique_ptr<AbstractExecutor> &&child_executor);

  const Schema *GetOutputSchema() override { return plan_->OutputSchema(); };

  void Init() override;

  // Note that Delete does not make use of the tuple pointer being passed in.
  // We throw exception if the delete failed for any reason, and return false if all delete succeeded.
  // Delete from indexes if necessary.
  bool Next([[maybe_unused]] Tuple *tuple, RID *rid) override;

 private:
  /** The delete plan node to be executed. */
  const DeletePlanNode *plan_;
  /** The child executor to obtain rid from. */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
