//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_join_executor.cpp
//
// Identification: src/execution/hash_join_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>
#include <vector>

#include "execution/executors/hash_join_executor.h"

namespace bustub {

HashJoinExecutor::HashJoinExecutor(ExecutorContext *exec_ctx, const HashJoinPlanNode *plan,
                                   std::unique_ptr<AbstractExecutor> &&left, std::unique_ptr<AbstractExecutor> &&right)
    : AbstractExecutor(exec_ctx) {}

/** @return the JHT in use. Do not modify this function, otherwise you will get a zero. */
// Uncomment me! const HT *GetJHT() const { return &jht_; }

void HashJoinExecutor::Init() {}

bool HashJoinExecutor::Next(Tuple *tuple) { return false; }
}  // namespace bustub
