//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// executor_factory.cpp
//
// Identification: src/execution/executor_factory.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executor_factory.h"

#include <memory>
#include <utility>

#include "execution/executors/abstract_executor.h"
#include "execution/executors/aggregation_executor.h"
#include "execution/executors/hash_join_executor.h"
#include "execution/executors/insert_executor.h"
#include "execution/executors/seq_scan_executor.h"

namespace bustub {
std::unique_ptr<AbstractExecutor> ExecutorFactory::CreateExecutor(ExecutorContext *exec_ctx,
                                                                  const AbstractPlanNode *plan) {
  switch (plan->GetType()) {
    // Create a new sequential scan executor.
    case PlanType::SeqScan: {
      return std::make_unique<SeqScanExecutor>(exec_ctx, dynamic_cast<const SeqScanPlanNode *>(plan));
    }

    // Create a new insert executor.
    case PlanType::Insert: {
      auto insert_plan = dynamic_cast<const InsertPlanNode *>(plan);
      auto child_executor =
          insert_plan->IsRawInsert() ? nullptr : ExecutorFactory::CreateExecutor(exec_ctx, insert_plan->GetChildPlan());
      return std::make_unique<InsertExecutor>(exec_ctx, insert_plan, std::move(child_executor));
    }

    // Create a new hash join executor.
    case PlanType::HashJoin: {
      auto join_plan = dynamic_cast<const HashJoinPlanNode *>(plan);
      auto left_executor = ExecutorFactory::CreateExecutor(exec_ctx, join_plan->GetLeftPlan());
      auto right_executor = ExecutorFactory::CreateExecutor(exec_ctx, join_plan->GetRightPlan());
      return std::make_unique<HashJoinExecutor>(exec_ctx, join_plan, std::move(left_executor),
                                                std::move(right_executor));
    }

    // Create a new aggregation executor.
    case PlanType::Aggregation: {
      auto agg_plan = dynamic_cast<const AggregationPlanNode *>(plan);
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, agg_plan->GetChildPlan());
      return std::make_unique<AggregationExecutor>(exec_ctx, agg_plan, std::move(child_executor));
    }

    default: {
      BUSTUB_ASSERT(false, "Unsupported plan type.");
    }
  }
}
}  // namespace bustub
