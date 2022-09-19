//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// projection_executor.h
//
// Identification: src/include/execution/executors/projection_executor.h
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * The ProjectionExecutor executor executes a projection.
 */
class ProjectionExecutor : public AbstractExecutor {
 public:
  /**
   * Construct a new ProjectionExecutor instance.
   * @param exec_ctx The executor context
   * @param plan The projection plan to be executed
   */
  ProjectionExecutor(ExecutorContext *exec_ctx, const ProjectionPlanNode *plan,
                     std::unique_ptr<AbstractExecutor> &&child_executor);

  /** Initialize the projection */
  void Init() override;

  /**
   * Yield the next tuple from the projection.
   * @param[out] tuple The next tuple produced by the projection
   * @param[out] rid The next tuple RID produced by the projection
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the projection plan */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

 private:
  /** The projection plan node to be executed */
  const ProjectionPlanNode *plan_;

  /** The child executor from which tuples are obtained */
  std::unique_ptr<AbstractExecutor> child_executor_;
};
}  // namespace bustub
