//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_plan.h
//
// Identification: src/include/execution/plans/update_plan.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * The UpdatePlanNode identifies a table that should be updated.
 * The tuple(s) to be updated come from the child of the UpdateExecutor.
 */
class UpdatePlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new UpdatePlanNode instance.
   * @param child The child plan to obtain tuple from
   * @param table_oid The identifier of the table that should be updated
   * @param target_expressions The target expressions for new tuples
   */
  UpdatePlanNode(SchemaRef output, AbstractPlanNodeRef child, table_oid_t table_oid,
                 std::vector<AbstractExpressionRef> target_expressions)
      : AbstractPlanNode(std::move(output), {std::move(child)}),
        table_oid_{table_oid},
        target_expressions_(std::move(target_expressions)) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Update; }

  /** @return The identifier of the table that should be updated */
  auto GetTableOid() const -> table_oid_t { return table_oid_; }

  /** @return The child plan providing tuples to be inserted */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "UPDATE should have exactly one child plan.");
    return GetChildAt(0);
  }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(UpdatePlanNode);

  /** The table to be updated. */
  table_oid_t table_oid_;

  /** The new expression at each column */
  std::vector<AbstractExpressionRef> target_expressions_;

 protected:
  auto PlanNodeToString() const -> std::string override;
};

}  // namespace bustub
