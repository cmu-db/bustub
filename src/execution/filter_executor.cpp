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
 * Yield the next tuple from the filter.
 * @param[out] tuple The next tuple produced by the filter
 * @param[out] rid The next tuple RID produced by the filter
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto FilterExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  auto filter_expr = plan_->GetPredicate();

  while (true) {
    // Get the next tuple
    const auto status = child_executor_->Next(tuple, rid);

    if (!status) {
      return false;
    }

    auto value = filter_expr->Evaluate(tuple, child_executor_->GetOutputSchema());
    if (!value.IsNull() && value.GetAs<bool>()) {
      return true;
    }
  }
}

}  // namespace bustub
