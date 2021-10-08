//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// delete_plan.h
//
// Identification: src/include/execution/plans/delete_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

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
  DeletePlanNode(const AbstractPlanNode *child, table_oid_t table_oid)
      : AbstractPlanNode(nullptr, {child}), table_oid_{table_oid} {}

  /** @return The type of the plan node */
  PlanType GetType() const override { return PlanType::Delete; }

  /** @return The identifier of the table from which tuples are deleted*/
  table_oid_t TableOid() const { return table_oid_; }

  /** @return The child plan providing tuples to be deleted */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "delete should have at most one child plan.");
    return GetChildAt(0);
  }

 private:
  /** The identifier of the table from which tuples are deleted */
  table_oid_t table_oid_;
};

}  // namespace bustub
