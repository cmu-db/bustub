//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_plan.h
//
// Identification: src/include/execution/plans/topn_plan.h
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
 * The TopNPlanNode represents a top-n operation. It will gather the n extreme rows based on
 * limit and order expressions.
 */
class TopNPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new TopNPlanNode instance.
   * @param output The output schema of this topN plan node
   * @param child The child plan node
   */
  TopNPlanNode(const Schema *output, const AbstractPlanNode *child) : AbstractPlanNode(output, {child}) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::TopN; }

  /** @return The child plan node */
  auto GetChildPlan() const -> const AbstractPlanNode * {
    BUSTUB_ASSERT(GetChildren().size() == 1, "TopN should have exactly one child plan.");
    return GetChildAt(0);
  }

 protected:
  auto PlanNodeToString() const -> std::string override { return fmt::format("TopN {{ }}"); }

 private:
};

}  // namespace bustub
