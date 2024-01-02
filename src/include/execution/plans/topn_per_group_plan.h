//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// topn_per_group_plan.h
//
// Identification: src/include/execution/plans/topn_per_group_plan.h
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
 * The TopNPerGroupPlanNode represents a top-n operation. It will gather the n extreme rows based on
 * limit and order expressions.
 */
class TopNPerGroupPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new TopNPerGroupPlanNode instance.
   * @param output The output schema of this TopNPerGroup plan node
   * @param child The child plan node
   * @param order_bys The sort expressions and their order by types.
   * @param n Retain n elements.
   */
  TopNPerGroupPlanNode(SchemaRef output, AbstractPlanNodeRef child, std::vector<AbstractExpressionRef> group_bys,
                       std::vector<std::pair<OrderByType, AbstractExpressionRef>> order_bys, std::size_t n)
      : AbstractPlanNode(std::move(output), {std::move(child)}),
        order_bys_(std::move(order_bys)),
        group_bys_(std::move(group_bys)),
        n_{n} {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::TopNPerGroup; }

  /** @return The N (limit) */
  auto GetN() const -> size_t { return n_; }

  /** @return Get order by expressions */
  auto GetOrderBy() const -> const std::vector<std::pair<OrderByType, AbstractExpressionRef>> & { return order_bys_; }

  /** @return Get group by expressions */
  auto GetGroupBy() const -> const std::vector<AbstractExpressionRef> & { return group_bys_; }

  /** @return The child plan node */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "TopNPerGroup should have exactly one child plan.");
    return GetChildAt(0);
  }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(TopNPerGroupPlanNode);

  std::vector<std::pair<OrderByType, AbstractExpressionRef>> order_bys_;
  std::vector<AbstractExpressionRef> group_bys_;
  std::size_t n_;

 protected:
  auto PlanNodeToString() const -> std::string override { return "TopNPerGroupPlan PlanNodeToString Not Implemented"; };
};

}  // namespace bustub
