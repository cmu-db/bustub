//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limit_plan.h
//
// Identification: src/include/execution/plans/limit_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * Limit constraints the number of output tuples produced by its child executor.
 */
class LimitPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new LimitPlanNode instance.
   * @param child The child plan from which tuples are obtained
   * @param limit The number of output tuples
   */
  LimitPlanNode(const Schema *output_schema, const AbstractPlanNode *child, std::size_t limit)
      : AbstractPlanNode(output_schema, {child}), limit_{limit} {}

  /** @return The type of the plan node */
  PlanType GetType() const override { return PlanType::Limit; }

  /** @return The limit */
  size_t GetLimit() const { return limit_; }

  /** @return The child plan node */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Limit should have at most one child plan.");
    return GetChildAt(0);
  }

 private:
  /** The limit */
  std::size_t limit_;
};

}  // namespace bustub
