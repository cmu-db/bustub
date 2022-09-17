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
#include <utility>
#include <vector>

#include "binder/bound_order_by.h"
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
   * @param order_by The sort expressions and their order by types.
   */
  SortPlanNode(const Schema *output, const AbstractPlanNode *child,
               std::vector<std::pair<OrderByType, const AbstractExpression *>> order_bys)
      : AbstractPlanNode(output, {child}), order_bys_(std::move(order_bys)) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Sort; }

  /** @return The child plan node */
  auto GetChildPlan() const -> const AbstractPlanNode * {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Sort should have exactly one child plan.");
    return GetChildAt(0);
  }

  /** @return Get sort by expressions */
  auto GetOrderBy() -> const std::vector<std::pair<OrderByType, const AbstractExpression *>> & { return order_bys_; }

 protected:
  auto PlanNodeToString() const -> std::string override;

 private:
  std::vector<std::pair<OrderByType, const AbstractExpression *>> order_bys_;
};

}  // namespace bustub
