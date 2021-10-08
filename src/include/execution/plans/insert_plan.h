//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// insert_plan.h
//
// Identification: src/include/execution/plans/insert_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <utility>
#include <vector>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * The InsertPlanNode identifies a table into which tuples are inserted.
 *
 * The values to be inserted are either embedded into the InsertPlanNode
 * itself, i.e. a "raw insert", or will come from the child of the node.
 *
 * NOTE: To simplify the assignment, InsertPlanNode has at most one child.
 */
class InsertPlanNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new insert plan node for inserting raw values.
   * @param raw_values the raw values to be inserted
   * @param table_oid the identifier of the table to be inserted into
   */
  InsertPlanNode(std::vector<std::vector<Value>> &&raw_values, table_oid_t table_oid)
      : AbstractPlanNode(nullptr, {}), raw_values_{std::move(raw_values)}, table_oid_{table_oid} {}

  /**
   * Creates a new insert plan node for inserting values from a child plan.
   * @param child the child plan to obtain values from
   * @param table_oid the identifier of the table that should be inserted into
   */
  InsertPlanNode(const AbstractPlanNode *child, table_oid_t table_oid)
      : AbstractPlanNode(nullptr, {child}), table_oid_(table_oid) {}

  /** @return The type of the plan node */
  PlanType GetType() const override { return PlanType::Insert; }

  /** @return The identifier of the table into which tuples are inserted */
  table_oid_t TableOid() const { return table_oid_; }

  /** @return `true` if we embed insert values directly into the plan, `false` if we have a child plan that provides
   * tuples*/
  bool IsRawInsert() const { return GetChildren().empty(); }

  /** @return The raw value to be inserted at the particular index */
  const std::vector<Value> &RawValuesAt(uint32_t idx) const {
    BUSTUB_ASSERT(IsRawInsert(), "This is not a raw insert, you should use the child plan.");
    return raw_values_[idx];
  }

  /** @return The raw values to be inserted */
  const std::vector<std::vector<Value>> &RawValues() const {
    BUSTUB_ASSERT(IsRawInsert(), "This is not a raw insert, you should use the child plan.");
    return raw_values_;
  }

  /** @return the child plan providing tuples to be inserted */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(!IsRawInsert(), "This is a raw insert, no child plan should be used.");
    BUSTUB_ASSERT(GetChildren().size() == 1, "Insert should have at most one child plan.");
    return GetChildAt(0);
  }

 private:
  /** The raw values embedded in this insert plan (may be empty) */
  std::vector<std::vector<Value>> raw_values_;
  /** The table to be inserted into. */
  table_oid_t table_oid_;
};

}  // namespace bustub
