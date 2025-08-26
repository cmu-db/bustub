//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_per_group_executor.cpp
//
// Identification: src/execution/topn_per_group_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/topn_per_group_executor.h"

namespace bustub {

/**
 * Construct a new TopNPerGroupExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The TopNPerGroup plan to be executed
 */
TopNPerGroupExecutor::TopNPerGroupExecutor(ExecutorContext *exec_ctx, const TopNPerGroupPlanNode *plan,
                                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

/** Initialize the TopNPerGroup */
void TopNPerGroupExecutor::Init() { throw NotImplementedException("TopNPerGroupExecutor is not implemented"); }

/**
 * Yield the next tuple from the TopNPerGroup.
 * @param[out] tuple The next tuple produced by the TopNPerGroup
 * @param[out] rid The next tuple RID produced by the TopNPerGroup
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto TopNPerGroupExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
