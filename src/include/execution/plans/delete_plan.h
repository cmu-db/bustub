//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// delete_plan.h
//
// Identification: src/include/execution/plans/delete_plan.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {
/**
 * DeletePlanNode identifies a table that should be deleted from.
 * The tuple/tuples to be deleted come from the child of the DeletedPlanNode. The child could either be an index scan
 * or a seq scan. To simplify the assignment, DeletePlanNode has at most one child.
 */
class DeletePlanNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new delete plan node that has a child plan.
   * @param child the child plan to obtain tuple from
   * @param table_oid the identifier of the table that should be deleted from
   */
  DeletePlanNode(const AbstractPlanNode *child, table_oid_t table_oid)
      : AbstractPlanNode(nullptr, {child}), table_oid_(table_oid) {}

  PlanType GetType() const override { return PlanType::Delete; }

  /** @return the identifier of the table that should be deleted from */
  table_oid_t TableOid() const { return table_oid_; }

  /** @return the child plan providing tuples to be inserted */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "delete should have at most one child plan.");
    return GetChildAt(0);
  }

 private:
  /** The table to be deleted from. */
  table_oid_t table_oid_;
};
}  // namespace bustub
