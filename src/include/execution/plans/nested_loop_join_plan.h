//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nested_loop_join.h
//
// Identification: src/include/execution/plans/nested_loop_join.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "binder/table_ref/bound_join_ref.h"
#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "fmt/core.h"

namespace bustub {

/**
 * NestedLoopJoinPlanNode joins tuples from two child plan nodes.
 */
class NestedLoopJoinPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new NestedLoopJoinPlanNode instance.
   * @param output The output format of this nested loop join node
   * @param children Two sequential scan children plans
   * @param predicate The predicate to join with, the tuples are joined
   * if predicate(tuple) = true.
   */
  NestedLoopJoinPlanNode(SchemaRef output_schema, AbstractPlanNodeRef left, AbstractPlanNodeRef right,
                         AbstractExpressionRef predicate, JoinType join_type)
      : AbstractPlanNode(std::move(output_schema), {std::move(left), std::move(right)}),
        predicate_(std::move(predicate)),
        join_type_(join_type) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::NestedLoopJoin; }

  /** @return The predicate to be used in the nested loop join */
  auto Predicate() const -> const AbstractExpression & { return *predicate_; }

  /** @return The join type used in the nested loop join */
  auto GetJoinType() const -> JoinType { return join_type_; };

  /** @return The left plan node of the nested loop join, by convention it should be the smaller table */
  auto GetLeftPlan() const -> AbstractPlanNodeRef { return GetChildAt(0); }

  /** @return The right plan node of the nested loop join */
  auto GetRightPlan() const -> AbstractPlanNodeRef { return GetChildAt(1); }

  static auto InferJoinSchema(const AbstractPlanNode &left, const AbstractPlanNode &right) -> Schema;

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(NestedLoopJoinPlanNode);

  /** The join predicate */
  AbstractExpressionRef predicate_;

  /** The join type */
  JoinType join_type_;

 protected:
  auto PlanNodeToString() const -> std::string override {
    return fmt::format("NestedLoopJoin {{ type={}, predicate={} }}", join_type_, predicate_);
  }
};

}  // namespace bustub
