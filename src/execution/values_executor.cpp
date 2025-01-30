//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// values_executor.cpp
//
// Identification: src/execution/values_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/values_executor.h"

namespace bustub {

/**
 * Construct a new ValuesExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The values plan to be executed
 */
ValuesExecutor::ValuesExecutor(ExecutorContext *exec_ctx, const ValuesPlanNode *plan)
    : AbstractExecutor(exec_ctx), plan_(plan), dummy_schema_(Schema({})) {}

/** Initialize the values */
void ValuesExecutor::Init() { cursor_ = 0; }

/**
 * Yield the next tuple from the values.
 * @param[out] tuple The next tuple produced by the values
 * @param[out] rid The next tuple RID produced by the values, not used by values executor
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto ValuesExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  if (cursor_ >= plan_->GetValues().size()) {
    return false;
  }

  std::vector<Value> values{};
  values.reserve(GetOutputSchema().GetColumnCount());

  const auto &row_expr = plan_->GetValues()[cursor_];
  for (const auto &col : row_expr) {
    values.push_back(col->Evaluate(nullptr, dummy_schema_));
  }

  *tuple = Tuple{values, &GetOutputSchema()};
  cursor_ += 1;

  return true;
}

}  // namespace bustub
