//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_per_group_executor.cpp
//
// Identification: src/execution/topn_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/topn_per_group_executor.h"

namespace bustub {

TopNPerGroupExecutor::TopNPerGroupExecutor(ExecutorContext *exec_ctx, const TopNPerGroupPlanNode *plan,
                                           std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx), plan_(plan), child_executor_(std::move(child_executor)) {}

void TopNPerGroupExecutor::Init() { throw NotImplementedException("TopNPerGroupExecutor is not implemented"); }

auto TopNPerGroupExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

}  // namespace bustub
