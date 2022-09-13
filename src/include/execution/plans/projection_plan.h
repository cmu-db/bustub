//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// projection_plan.h
//
// Identification: src/include/execution/plans/projection_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * The ProjectionPlanNode represents a project operation.
 * It computes expressions based on the input.
 */
class ProjectionPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new ProjectionPlanNode instance.
   * @param output The output schema of this projection node
   * @param expressions The expression to evaluate
   * @param child The child plan node
   */
  ProjectionPlanNode(const Schema *output, std::vector<const AbstractExpression *> expressions,
                     const AbstractPlanNode *child)
      : AbstractPlanNode(output, {child}), expressions_(std::move(expressions)) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Projection; }

  /** @return The child plan node */
  auto GetChildPlan() const -> const AbstractPlanNode * {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Projection should have exactly one child plan.");
    return GetChildAt(0);
  }

  /** @return Projection expressions */
  auto GetExpressions() const -> const std::vector<const AbstractExpression *> & { return expressions_; }

 protected:
  auto PlanNodeToString() const -> std::string override;

 private:
  std::vector<const AbstractExpression *> expressions_;
};

}  // namespace bustub
