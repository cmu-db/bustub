//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// sort_plan.h
//
// Identification: src/include/execution/plans/sort_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * The SortPlanNode represents a sort operation. It will sort the input with
 * the given predicate.
 */
class SortPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new SortPlanNode instance.
   * @param output The output schema of this sort plan node
   * @param child The child plan node
   */
  SortPlanNode(const Schema *output, const AbstractPlanNode *child) : AbstractPlanNode(output, {child}) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Sort; }

  /** @return The child plan node */
  auto GetChildPlan() const -> const AbstractPlanNode * {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Sort should have exactly one child plan.");
    return GetChildAt(0);
  }

 protected:
  auto PlanNodeToString() const -> std::string override { return fmt::format("Sort {{ }}"); }

 private:
};

}  // namespace bustub
