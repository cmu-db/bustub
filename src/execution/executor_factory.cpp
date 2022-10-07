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
#include "execution/executors/filter_executor.h"
#include "execution/executors/hash_join_executor.h"
#include "execution/executors/index_scan_executor.h"
#include "execution/executors/insert_executor.h"
#include "execution/executors/limit_executor.h"
#include "execution/executors/mock_scan_executor.h"
#include "execution/executors/nested_index_join_executor.h"
#include "execution/executors/nested_loop_join_executor.h"
#include "execution/executors/projection_executor.h"
#include "execution/executors/seq_scan_executor.h"
#include "execution/executors/sort_executor.h"
#include "execution/executors/topn_executor.h"
#include "execution/executors/update_executor.h"
#include "execution/executors/values_executor.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/mock_scan_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/sort_plan.h"
#include "execution/plans/topn_plan.h"
#include "execution/plans/values_plan.h"
#include "storage/index/generic_key.h"

namespace bustub {

auto ExecutorFactory::CreateExecutor(ExecutorContext *exec_ctx, const AbstractPlanNodeRef &plan)
    -> std::unique_ptr<AbstractExecutor> {
  switch (plan->GetType()) {
    // Create a new sequential scan executor
    case PlanType::SeqScan: {
      return std::make_unique<SeqScanExecutor>(exec_ctx, dynamic_cast<const SeqScanPlanNode *>(plan.get()));
    }

    // Create a new index scan executor
    case PlanType::IndexScan: {
      return std::make_unique<IndexScanExecutor>(exec_ctx, dynamic_cast<const IndexScanPlanNode *>(plan.get()));
    }

    // Create a new insert executor
    case PlanType::Insert: {
      auto insert_plan = dynamic_cast<const InsertPlanNode *>(plan.get());
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, insert_plan->GetChildPlan());
      return std::make_unique<InsertExecutor>(exec_ctx, insert_plan, std::move(child_executor));
    }

    // Create a new update executor
    case PlanType::Update: {
      auto update_plan = dynamic_cast<const UpdatePlanNode *>(plan.get());
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, update_plan->GetChildPlan());
      return std::make_unique<UpdateExecutor>(exec_ctx, update_plan, std::move(child_executor));
    }

    // Create a new delete executor
    case PlanType::Delete: {
      auto delete_plan = dynamic_cast<const DeletePlanNode *>(plan.get());
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, delete_plan->GetChildPlan());
      return std::make_unique<DeleteExecutor>(exec_ctx, delete_plan, std::move(child_executor));
    }

    // Create a new limit executor
    case PlanType::Limit: {
      auto limit_plan = dynamic_cast<const LimitPlanNode *>(plan.get());
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, limit_plan->GetChildPlan());
      return std::make_unique<LimitExecutor>(exec_ctx, limit_plan, std::move(child_executor));
    }

    // Create a new aggregation executor
    case PlanType::Aggregation: {
      auto agg_plan = dynamic_cast<const AggregationPlanNode *>(plan.get());
      auto child_executor = ExecutorFactory::CreateExecutor(exec_ctx, agg_plan->GetChildPlan());
      return std::make_unique<AggregationExecutor>(exec_ctx, agg_plan, std::move(child_executor));
    }

    // Create a new nested-loop join executor
    case PlanType::NestedLoopJoin: {
      auto nested_loop_join_plan = dynamic_cast<const NestedLoopJoinPlanNode *>(plan.get());
      auto left = ExecutorFactory::CreateExecutor(exec_ctx, nested_loop_join_plan->GetLeftPlan());
      auto right = ExecutorFactory::CreateExecutor(exec_ctx, nested_loop_join_plan->GetRightPlan());
      return std::make_unique<NestedLoopJoinExecutor>(exec_ctx, nested_loop_join_plan, std::move(left),
                                                      std::move(right));
    }

    // Create a new nested-index join executor
    case PlanType::NestedIndexJoin: {
      auto nested_index_join_plan = dynamic_cast<const NestedIndexJoinPlanNode *>(plan.get());
      auto left = ExecutorFactory::CreateExecutor(exec_ctx, nested_index_join_plan->GetChildPlan());
      return std::make_unique<NestIndexJoinExecutor>(exec_ctx, nested_index_join_plan, std::move(left));
    }

    // Create a new hash join executor
    case PlanType::HashJoin: {
      auto hash_join_plan = dynamic_cast<const HashJoinPlanNode *>(plan.get());
      auto left = ExecutorFactory::CreateExecutor(exec_ctx, hash_join_plan->GetLeftPlan());
      auto right = ExecutorFactory::CreateExecutor(exec_ctx, hash_join_plan->GetRightPlan());
      return std::make_unique<HashJoinExecutor>(exec_ctx, hash_join_plan, std::move(left), std::move(right));
    }

    // Create a new mock scan executor
    case PlanType::MockScan: {
      const auto *mock_scan_plan = dynamic_cast<const MockScanPlanNode *>(plan.get());
      return std::make_unique<MockScanExecutor>(exec_ctx, mock_scan_plan);
    }

    // Create a new projection executor
    case PlanType::Projection: {
      const auto *projection_plan = dynamic_cast<const ProjectionPlanNode *>(plan.get());
      auto child = ExecutorFactory::CreateExecutor(exec_ctx, projection_plan->GetChildPlan());
      return std::make_unique<ProjectionExecutor>(exec_ctx, projection_plan, std::move(child));
    }

      // Create a new filter executor
    case PlanType::Filter: {
      const auto *filter_plan = dynamic_cast<const FilterPlanNode *>(plan.get());
      auto child = ExecutorFactory::CreateExecutor(exec_ctx, filter_plan->GetChildPlan());
      return std::make_unique<FilterExecutor>(exec_ctx, filter_plan, std::move(child));
    }

      // Create a new filter executor
    case PlanType::Values: {
      const auto *values_plan = dynamic_cast<const ValuesPlanNode *>(plan.get());
      return std::make_unique<ValuesExecutor>(exec_ctx, values_plan);
    }

      // Create a new sort executor
    case PlanType::Sort: {
      const auto *sort_plan = dynamic_cast<const SortPlanNode *>(plan.get());
      auto child = ExecutorFactory::CreateExecutor(exec_ctx, sort_plan->GetChildPlan());
      return std::make_unique<SortExecutor>(exec_ctx, sort_plan, std::move(child));
    }

      // Create a new topN executor
    case PlanType::TopN: {
      const auto *topn_plan = dynamic_cast<const TopNPlanNode *>(plan.get());
      auto child = ExecutorFactory::CreateExecutor(exec_ctx, topn_plan->GetChildPlan());
      return std::make_unique<TopNExecutor>(exec_ctx, topn_plan, std::move(child));
    }

    default:
      UNREACHABLE("Unsupported plan type.");
  }
}

}  // namespace bustub
