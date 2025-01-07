//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// projection_executor.cpp
//
// Identification: src/execution/projection_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/projection_executor.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * Construct a new ProjectionExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The projection plan to be executed
 */
ProjectionExecutor::ProjectionExecutor(ExecutorContext *exec_ctx, const ProjectionPlanNode *plan,
                                       std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

/** Initialize the projection */
void ProjectionExecutor::Init() {
  // Initialize the child executor
  child_executor_->Init();
}

/**
 * Yield the next tuple from the projection.
 * @param[out] tuple The next tuple produced by the projection
 * @param[out] rid The next tuple RID produced by the projection
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto ProjectionExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  Tuple child_tuple{};

  // Get the next tuple
  const auto status = child_executor_->Next(&child_tuple, rid);

  if (!status) {
    return false;
  }

  // Compute expressions
  std::vector<Value> values{};
  values.reserve(GetOutputSchema().GetColumnCount());
  for (const auto &expr : plan_->GetExpressions()) {
    values.push_back(expr->Evaluate(&child_tuple, child_executor_->GetOutputSchema()));
  }

  *tuple = Tuple{values, &GetOutputSchema()};

  return true;
}
}  // namespace bustub
