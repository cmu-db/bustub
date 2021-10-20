//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_plan.h
//
// Identification: src/include/execution/plans/update_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <unordered_map>
#include <utility>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/** The UpdateType enumeration describes the supported update operations */
enum class UpdateType { Add, Set };

/** Metadata about an Update. */
struct UpdateInfo {
  UpdateInfo(UpdateType type, int update_val) : type_{type}, update_val_{update_val} {}
  UpdateType type_;
  int update_val_;
};

/**
 * The UpdatePlanNode identifies a table that should be updated.
 * The tuple(s) to be updated come from the child of the UpdateExecutor.
 *
 * NOTE: To simplify the assignment, UpdatePlanNode has at most one child.
 */
class UpdatePlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new UpdatePlanNode instance.
   * @param child the child plan to obtain tuple from
   * @param table_oid the identifier of the table that should be updated
   */
  UpdatePlanNode(const AbstractPlanNode *child, table_oid_t table_oid,
                 std::unordered_map<uint32_t, UpdateInfo> update_attrs)
      : AbstractPlanNode(nullptr, {child}), table_oid_{table_oid}, update_attrs_{std::move(update_attrs)} {}

  /** @return The type of the plan node */
  PlanType GetType() const override { return PlanType::Update; }

  /** @return The identifier of the table that should be updated */
  table_oid_t TableOid() const { return table_oid_; }

  /** @return The child plan providing tuples to be inserted */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "UPDATE should have at most one child plan.");
    return GetChildAt(0);
  }

  /** @return The update attributes */
  const std::unordered_map<uint32_t, UpdateInfo> &GetUpdateAttr() const { return update_attrs_; }

 private:
  /** The table to be updated. */
  table_oid_t table_oid_;
  /** Map from column index -> update operation */
  const std::unordered_map<uint32_t, UpdateInfo> update_attrs_;
};

}  // namespace bustub
