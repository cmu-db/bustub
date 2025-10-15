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
 * Yield the next tuple batch from the values.
 * @param[out] tuple_batch The next tuple batch produced by the values
 * @param[out] rid_batch The next tuple RID batch produced by the values
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto ValuesExecutor::Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch,
                          size_t batch_size) -> bool {
  tuple_batch->clear();
  rid_batch->clear();

  while (tuple_batch->size() < batch_size && cursor_ < plan_->GetValues().size()) {
    std::vector<Value> values{};
    values.reserve(GetOutputSchema().GetColumnCount());

    const auto &row_expr = plan_->GetValues()[cursor_];
    for (const auto &col : row_expr) {
      values.push_back(col->Evaluate(nullptr, dummy_schema_));
    }

    tuple_batch->emplace_back(values, &GetOutputSchema());
    rid_batch->emplace_back(RID{});
    cursor_ += 1;
  }
  return !tuple_batch->empty();
}

}  // namespace bustub
