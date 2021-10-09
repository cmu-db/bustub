//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// distinct_plan.h
//
// Identification: src/include/execution/plans/distinct_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * Distinct removes duplicate rows from the output of a child node.
 */
class DistinctPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new DistinctPlanNode instance.
   * @param child The child plan from which tuples are obtained
   */
  DistinctPlanNode(const Schema *output_schema, const AbstractPlanNode *child)
      : AbstractPlanNode(output_schema, {child}) {}

  /** @return The type of the plan node */
  PlanType GetType() const override { return PlanType::Distinct; }

  /** @return The child plan node */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Distinct should have at most one child plan.");
    return GetChildAt(0);
  }
};

}  // namespace bustub
