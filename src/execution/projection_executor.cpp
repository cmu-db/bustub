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
 * Yield the next tuple batch from the projection.
 * @param[out] tuple_batch The next tuple batch produced by the projection
 * @param[out] rid_batch The next tuple RID batch produced by the projection
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto ProjectionExecutor::Next(std::vector<Tuple> *tuple_batch, std::vector<RID> *rid_batch, size_t batch_size) -> bool {
  tuple_batch->clear();
  rid_batch->clear();

  if (child_offset_ != 0) {
    for (size_t i = child_offset_; i < child_tuples_.size(); i++) {
      auto child_tuple = child_tuples_[i];
      auto child_rid = child_rids_[i];

      // Compute expressions
      std::vector<Value> values{};
      values.reserve(GetOutputSchema().GetColumnCount());
      for (const auto &expr : plan_->GetExpressions()) {
        values.push_back(expr->Evaluate(&child_tuple, child_executor_->GetOutputSchema()));
      }

      tuple_batch->push_back(Tuple{values, &GetOutputSchema()});
      rid_batch->push_back(child_rid);
    }
  }

  child_offset_ = 0;

  const auto status = child_executor_->Next(&child_tuples_, &child_rids_, batch_size);

  // If no more tuples and output batch is empty, return false
  if (!status && tuple_batch->empty()) {
    return false;
  }

  // If no more tuples but output batch is not empty, return true
  if (!status && !tuple_batch->empty()) {
    return true;
  }

  for (size_t i = 0; i < child_tuples_.size(); i++) {
    auto child_tuple = child_tuples_[i];
    auto child_rid = child_rids_[i];

    // Compute expressions
    std::vector<Value> values{};
    values.reserve(GetOutputSchema().GetColumnCount());
    for (const auto &expr : plan_->GetExpressions()) {
      values.push_back(expr->Evaluate(&child_tuple, child_executor_->GetOutputSchema()));
    }

    tuple_batch->push_back(Tuple{values, &GetOutputSchema()});
    rid_batch->push_back(child_rid);

    if (tuple_batch->size() >= batch_size) {
      // If we have filled the output batch but not yet reached the end of the current child batch, update the offset
      // and return
      if (i + 1 < child_tuples_.size()) {
        child_offset_ = i + 1;
      } else {
        child_offset_ = 0;
      }

      return true;
    }
  }

  return !tuple_batch->empty();
}
}  // namespace bustub
