//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// executor_factory.cpp
//
// Identification: src/execution/executor_factory.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executor_factory.h"

#include <memory>
#include <utility>

#include "execution/executors/abstract_executor.h"
#include "execution/executors/aggregation_executor.h"
#include "execution/executors/delete_executor.h"
#include "execution/executors/distinct_executor.h"
#include "execution/executors/hash_join_executor.h"
#include "execution/executors/index_scan_executor.h"
#include "execution/executors/insert_executor.h"
#include "execution/executors/limit_executor.h"
#include "execution/executors/nested_index_join_executor.h"
#include "execution/executors/nested_loop_join_executor.h"
#include "execution/executors/seq_scan_executor.h"
#include "execution/executors/update_executor.h"
#include "storage/index/generic_key.h"

namespace bustub {

std::unique_ptr<AbstractExecutor> ExecutorFactory::CreateExecutor(ExecutorContext *exec_ctx,
                                                                  const AbstractPlanNode *plan) {
  switch (plan->GetType()) {
    // Create a new sequential scan executor
    case PlanType::SeqScan: {
      return std::make_unique<SeqScanExecutor>(exec_ctx, dynamic_cast<const SeqScanPlanNode *>(plan));
    }

    // Create a new index scan executor
    case PlanType::IndexScan: {
      return std::make_unique<IndexScanExecutor>(exec_ctx, dynamic_cast<const IndexScanPlanNode *>(plan));
    }

    // Create a new insert executor
    case PlanType::Insert: {
      auto insert_plan = dynamic_cast<const InsertPlanNode *>(plan);
      auto child_executor =
          insert_plan->IsRawInsert() ? nullptr : ExecutorFactory::CreateExecutor(exec_ctx, insert_plan->GetChildPlan());
      return std::make_unique<InsertExecutor>(exec_ctx, insert_plan, std::move(child_executor));
    }

    // Create a new update executor
    case PlanType::Update: {
      auto update_plan = dynamic_cast<const UpdatePlanNode *>(plan);
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, update_plan->GetChildPlan());
      return std::make_unique<UpdateExecutor>(exec_ctx, update_plan, std::move(child_executor));
    }

    // Create a new delete executor
    case PlanType::Delete: {
      auto delete_plan = dynamic_cast<const DeletePlanNode *>(plan);
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, delete_plan->GetChildPlan());
      return std::make_unique<DeleteExecutor>(exec_ctx, delete_plan, std::move(child_executor));
    }

    // Create a new limit executor
    case PlanType::Limit: {
      auto limit_plan = dynamic_cast<const LimitPlanNode *>(plan);
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, limit_plan->GetChildPlan());
      return std::make_unique<LimitExecutor>(exec_ctx, limit_plan, std::move(child_executor));
    }

    // Create a new distinct executor
    case PlanType::Distinct: {
      auto distinct_plan = dynamic_cast<const DistinctPlanNode *>(plan);
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, distinct_plan->GetChildPlan());
      return std::make_unique<DistinctExecutor>(exec_ctx, distinct_plan, std::move(child_executor));
    }

    // Create a new aggregation executor
    case PlanType::Aggregation: {
      auto agg_plan = dynamic_cast<const AggregationPlanNode *>(plan);
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, agg_plan->GetChildPlan());
      return std::make_unique<AggregationExecutor>(exec_ctx, agg_plan, std::move(child_executor));
    }

    // Create a new nested-loop join executor
    case PlanType::NestedLoopJoin: {
      auto nested_loop_join_plan = dynamic_cast<const NestedLoopJoinPlanNode *>(plan);
      auto left = ExecutorFactory::CreateExecutor(exec_ctx, nested_loop_join_plan->GetLeftPlan());
      auto right = ExecutorFactory::CreateExecutor(exec_ctx, nested_loop_join_plan->GetRightPlan());
      return std::make_unique<NestedLoopJoinExecutor>(exec_ctx, nested_loop_join_plan, std::move(left),
                                                      std::move(right));
    }

    // Create a new nested-index join executor
    case PlanType::NestedIndexJoin: {
      auto nested_index_join_plan = dynamic_cast<const NestedIndexJoinPlanNode *>(plan);
      auto left = ExecutorFactory::CreateExecutor(exec_ctx, nested_index_join_plan->GetChildPlan());
      return std::make_unique<NestIndexJoinExecutor>(exec_ctx, nested_index_join_plan, std::move(left));
    }

    // Create a new hash join executor
    case PlanType::HashJoin: {
      auto hash_join_plan = dynamic_cast<const HashJoinPlanNode *>(plan);
      auto left = ExecutorFactory::CreateExecutor(exec_ctx, hash_join_plan->GetLeftPlan());
      auto right = ExecutorFactory::CreateExecutor(exec_ctx, hash_join_plan->GetRightPlan());
      return std::make_unique<HashJoinExecutor>(exec_ctx, hash_join_plan, std::move(left), std::move(right));
    }

    default:
      UNREACHABLE("Unsupported plan type.");
  }
}

}  // namespace bustub
