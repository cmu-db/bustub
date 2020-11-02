//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limit_plan.h
//
// Identification: src/include/execution/plans/limit_plan.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "execution/plans/abstract_plan.h"

namespace bustub {
/**
 * Limit constraints the number of output tuples produced by its child executor. The offset indicates the number
 * of rows to be skipped before returning.
 */
class LimitPlanNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new limit plan node that has a child plan.
   * @param child the child plan to obtain tuple from
   * @param limit the number of output tuples
   * @param offset the number of rows to be skipped
   */
  LimitPlanNode(const AbstractPlanNode *child, size_t limit, size_t offset)
      : AbstractPlanNode(nullptr, {child}), limit_(limit), offset_(offset) {}

  PlanType GetType() const override { return PlanType::Limit; }

  size_t GetLimit() const { return limit_; }

  size_t GetOffset() const { return offset_; }

  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Limit should have at most one child plan.");
    return GetChildAt(0);
  }

 private:
  size_t limit_;
  size_t offset_;
};
}  // namespace bustub
