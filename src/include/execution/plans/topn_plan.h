//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_plan.h
//
// Identification: src/include/execution/plans/topn_plan.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
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
 * The TopNPlanNode represents a top-n operation. It will gather the n extreme rows based on
 * limit and order expressions.
 */
class TopNPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new TopNPlanNode instance.
   * @param output The output schema of this TopN plan node
   * @param child The child plan node
   * @param order_bys The sort expressions and their order by types.
   * @param n Retain n elements.
   */
  TopNPlanNode(SchemaRef output, AbstractPlanNodeRef child, std::vector<OrderBy> order_bys, std::size_t n)
      : AbstractPlanNode(std::move(output), {std::move(child)}), order_bys_(std::move(order_bys)), n_{n} {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::TopN; }

  /** @return The N (limit) */
  auto GetN() const -> size_t { return n_; }

  /** @return Get order by expressions */
  auto GetOrderBy() const -> const std::vector<OrderBy> & { return order_bys_; }

  /** @return The child plan node */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "TopN should have exactly one child plan.");
    return GetChildAt(0);
  }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(TopNPlanNode);

  std::vector<OrderBy> order_bys_;
  std::size_t n_;

 protected:
  auto PlanNodeToString() const -> std::string override;
};

}  // namespace bustub
