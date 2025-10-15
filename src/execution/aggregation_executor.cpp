//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregation_executor.cpp
//
// Identification: src/execution/aggregation_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>
#include "common/macros.h"

#include "execution/executors/aggregation_executor.h"

namespace bustub {

/**
 * Construct a new AggregationExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The insert plan to be executed
 * @param child_executor The child executor from which inserted tuples are pulled (may be `nullptr`)
 */
AggregationExecutor::AggregationExecutor(ExecutorContext *exec_ctx, const AggregationPlanNode *plan,
                                         std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {
  UNIMPLEMENTED("TODO(P3): Add implementation.");
}

/** Initialize the aggregation */
void AggregationExecutor::Init() { UNIMPLEMENTED("TODO(P3): Add implementation."); }

/**
 * Yield the next tuple batch from the aggregation.
 * @param[out] tuple_batch The next batch of tuples produced by the aggregation
 * @param[out] rid_batch The next batch of tuple RIDs produced by the aggregation
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if any tuples were produced, `false` if there are no more tuples
 */

auto AggregationExecutor::Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch,
                               size_t batch_size) -> bool {
  UNIMPLEMENTED("TODO(P3): Add implementation.");
}

/** Do not use or remove this function; otherwise, you will get zero points. */
auto AggregationExecutor::GetChildExecutor() const -> const AbstractExecutor * { return child_executor_.get(); }

}  // namespace bustub
