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

#include <utility>
#include <vector>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

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
   * if predicate(tuple) = true or predicate = `nullptr`
   */
  NestedLoopJoinPlanNode(const Schema *output_schema, std::vector<const AbstractPlanNode *> &&children,
                         const AbstractExpression *predicate)
      : AbstractPlanNode(output_schema, std::move(children)), predicate_(predicate) {}

  /** @return The type of the plan node */
  PlanType GetType() const override { return PlanType::NestedLoopJoin; }

  /** @return The predicate to be used in the nested loop join */
  const AbstractExpression *Predicate() const { return predicate_; }

  /** @return The left plan node of the nested loop join, by convention it should be the smaller table */
  const AbstractPlanNode *GetLeftPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 2, "Nested loop joins should have exactly two children plans.");
    return GetChildAt(0);
  }

  /** @return The right plan node of the nested loop join */
  const AbstractPlanNode *GetRightPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 2, "Nested loop joins should have exactly two children plans.");
    return GetChildAt(1);
  }

 private:
  /** The join predicate */
  const AbstractExpression *predicate_;
};

}  // namespace bustub
