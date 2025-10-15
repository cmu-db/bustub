//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// filter_executor.cpp
//
// Identification: src/execution/filter_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/filter_executor.h"
#include "common/exception.h"
#include "type/value_factory.h"

namespace bustub {

/**
 * Construct a new FilterExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The filter plan to be executed
 * @param child_executor The child executor that feeds the filter
 */
FilterExecutor::FilterExecutor(ExecutorContext *exec_ctx, const FilterPlanNode *plan,
                               std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

/** Initialize the filter */
void FilterExecutor::Init() {
  // Initialize the child executor
  child_executor_->Init();
}

/**
 * Yield the next tuple batch from the filter.
 * @param[out] tuple_batch The next tuple batch produced by the filter
 * @param[out] rid_batch The next tuple RID batch produced by the filter
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto FilterExecutor::Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch,
                          size_t batch_size) -> bool {
  tuple_batch->clear();
  rid_batch->clear();

  auto filter_expr = plan_->GetPredicate();

  while (true) {
    // If the child offset is not zero, process remaining tuples in the last fetched batch
    if (child_offset_ != 0) {
      for (size_t i = child_offset_; i < child_tuples_.size(); ++i) {
        auto &tuple = child_tuples_[i];
        auto &rid = child_rids_[i];
        // Evaluate the filter predicate
        auto value = filter_expr->Evaluate(&tuple, child_executor_->GetOutputSchema());
        if (filter_expr == nullptr || (!value.IsNull() && value.GetAs<bool>())) {
          tuple_batch->push_back(tuple);
          rid_batch->push_back(rid);
        }
      }
    }

    child_offset_ = 0;

    // Get the next tuple batch from the child executor
    const auto status = child_executor_->Next(&child_tuples_, &child_rids_, batch_size);

    // If no more tuples and output batch is empty, return false
    if (!status && tuple_batch->empty()) {
      return false;
    }

    // If no more tuples but output batch is not empty, return true
    if (!status && !tuple_batch->empty()) {
      return true;
    }

    for (size_t i = 0; i < child_tuples_.size(); ++i) {
      auto &tuple = child_tuples_[i];
      auto &rid = child_rids_[i];
      // Evaluate the filter predicate
      auto value = filter_expr->Evaluate(&tuple, child_executor_->GetOutputSchema());
      if (filter_expr == nullptr || (!value.IsNull() && value.GetAs<bool>())) {
        tuple_batch->push_back(tuple);
        rid_batch->push_back(rid);
        if (tuple_batch->size() >= batch_size) {
          // If we have filled the output batch but not yet reached the end of the current child batch, update the
          // offset and return
          if (i + 1 < child_tuples_.size()) {
            child_offset_ = i + 1;
          } else {
            child_offset_ = 0;
          }

          return true;
        }
      }
    }
  }
}

}  // namespace bustub
