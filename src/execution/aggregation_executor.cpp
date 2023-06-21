//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregation_executor.cpp
//
// Identification: src/execution/aggregation_executor.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>
#include <vector>

#include "execution/executors/aggregation_executor.h"

namespace bustub {

AggregationExecutor::AggregationExecutor(ExecutorContext *exec_ctx, const AggregationPlanNode *plan,
                                         std::unique_ptr<AbstractExecutor> &&child_executor)
    : AbstractExecutor(exec_ctx) {}

void AggregationExecutor::Init() {}

auto AggregationExecutor::Next(Tuple *tuple, RID *rid) -> bool { return false; }

auto AggregationExecutor::GetChildExecutor() const -> const AbstractExecutor * { return child_executor_.get(); }

}  // namespace bustub
