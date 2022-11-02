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

#include <string>
#include <utility>

#include "execution/plans/abstract_plan.h"
#include "fmt/format.h"

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
  LimitPlanNode(SchemaRef output, AbstractPlanNodeRef child, std::size_t limit)
      : AbstractPlanNode(std::move(output), {std::move(child)}), limit_{limit} {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Limit; }

  /** @return The limit */
  auto GetLimit() const -> size_t { return limit_; }

  /** @return The child plan node */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Limit should have at most one child plan.");
    return GetChildAt(0);
  }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(LimitPlanNode);

  /** The limit */
  std::size_t limit_;

 protected:
  auto PlanNodeToString() const -> std::string override;
};

}  // namespace bustub
