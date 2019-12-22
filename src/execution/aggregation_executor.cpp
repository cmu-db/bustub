//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregation_executor.cpp
//
// Identification: src/execution/aggregation_executor.cpp
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <memory>
#include <vector>

#include "execution/executors/aggregation_executor.h"

namespace bustub {

AggregationExecutor::AggregationExecutor(ExecutorContext *exec_ctx, const AggregationPlanNode *plan,
                      std::unique_ptr<AbstractExecutor> &&child)
      : AbstractExecutor(exec_ctx) {}

const AbstractExecutor *AggregationExecutor::GetChildExecutor() const { return child_.get(); }

const Schema *AggregationExecutor::GetOutputSchema() { return plan_->OutputSchema(); }

void AggregationExecutor::Init() {}

bool AggregationExecutor::Next(Tuple *tuple) { return false; }

  /** @return the tuple as an AggregateKey */
AggregateKey AggregationExecutor::MakeKey(const Tuple *tuple) {
    std::vector<Value> keys;
    for (const auto &expr : plan_->GetGroupBys()) {
      keys.emplace_back(expr->Evaluate(tuple, child_->GetOutputSchema()));
    }
    return {keys};
  }

  /** @return the tuple as an AggregateValue */
AggregateValue AggregationExecutor::MakeVal(const Tuple *tuple) {
    std::vector<Value> vals;
    for (const auto &expr : plan_->GetAggregates()) {
      vals.emplace_back(expr->Evaluate(tuple, child_->GetOutputSchema()));
    }
    return {vals};
  }
}  // namespace bustub
