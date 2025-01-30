//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_executor.cpp
//
// Identification: src/execution/topn_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/topn_executor.h"

namespace bustub {

/**
 * Construct a new TopNExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The TopN plan to be executed
 */
TopNExecutor::TopNExecutor(ExecutorContext *exec_ctx, const TopNPlanNode *plan,
                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {}

/** Initialize the TopN */
void TopNExecutor::Init() { throw NotImplementedException("TopNExecutor is not implemented"); }

/**
 * Yield the next tuple from the TopN.
 * @param[out] tuple The next tuple produced by the TopN
 * @param[out] rid The next tuple RID produced by the TopN
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto TopNExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

auto TopNExecutor::GetNumInHeap() -> size_t { throw NotImplementedException("TopNExecutor is not implemented"); };

}  // namespace bustub
