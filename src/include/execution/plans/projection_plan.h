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
#include "catalog/schema.h"
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
  ProjectionPlanNode(SchemaRef output, std::vector<AbstractExpressionRef> expressions, AbstractPlanNodeRef child)
      : AbstractPlanNode(std::move(output), {std::move(child)}), expressions_(std::move(expressions)) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Projection; }

  /** @return The child plan node */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Projection should have exactly one child plan.");
    return GetChildAt(0);
  }

  /** @return Projection expressions */
  auto GetExpressions() const -> const std::vector<AbstractExpressionRef> & { return expressions_; }

  static auto InferProjectionSchema(const std::vector<AbstractExpressionRef> &expressions) -> Schema;

  static auto RenameSchema(const Schema &schema, const std::vector<std::string> &col_names) -> Schema;

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(ProjectionPlanNode);

  std::vector<AbstractExpressionRef> expressions_;

 protected:
  auto PlanNodeToString() const -> std::string override;
};

}  // namespace bustub
