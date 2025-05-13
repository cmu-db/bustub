//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// delete_plan.h
//
// Identification: src/include/execution/plans/delete_plan.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <utility>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * The DeletePlanNode identifies a table from which tuples should be deleted.
 * The tuple(s) to be updated come from the child of the DeleteExecutor.
 *
 * NOTE: To simplify the assignment, DeletePlanNode has at most one child.
 */
class DeletePlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new DeletePlanNode.
   * @param child The child plan to obtain tuple from
   * @param table_oid The identifier of the table from which tuples are deleted
   */
  DeletePlanNode(SchemaRef output, AbstractPlanNodeRef child, table_oid_t table_oid)
      : AbstractPlanNode(std::move(output), {std::move(child)}), table_oid_{table_oid} {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Delete; }

  /** @return The identifier of the table from which tuples are deleted*/
  auto GetTableOid() const -> table_oid_t { return table_oid_; }

  /** @return The child plan providing tuples to be deleted */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "delete should have at most one child plan.");
    return GetChildAt(0);
  }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(DeletePlanNode);

  /** The identifier of the table from which tuples are deleted */
  table_oid_t table_oid_;

 protected:
  auto PlanNodeToString() const -> std::string override { return fmt::format("Delete {{ table_oid={} }}", table_oid_); }
};

}  // namespace bustub
