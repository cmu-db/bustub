//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_plan.h
//
// Identification: src/include/execution/plans/update_plan.h
//
// Copyright (c) 2015-20, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <unordered_map>
#include <utility>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

enum class UpdateType { Add, Set };

/**
 * Metadata about an Update.
 */
struct UpdateInfo {
  UpdateInfo(UpdateType type, int update_val) : type_(type), update_val_(update_val) {}
  UpdateType type_;
  int update_val_;
};

/**
 * UpdatePlannode identifies a table that should be updated.
 * The tuple/tuples to be updated come from the child of the UpdatePlanNode. The child could either be an index scan
 * or a seq scan. To simplify the assignment, UpdatePlannode has at most one child.
 */
class UpdatePlanNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new update plan node that has a child plan.
   * @param child the child plan to obtain tuple from
   * @param table_oid the identifier of the table that should be updated
   */
  UpdatePlanNode(const AbstractPlanNode *child, table_oid_t table_oid,
                 const std::unordered_map<uint32_t, UpdateInfo> &update_attrs)
      : AbstractPlanNode(nullptr, {child}), table_oid_(table_oid), update_attrs_(&update_attrs) {}

  PlanType GetType() const override { return PlanType::Update; }

  /** @return the identifier of the table that should be updated */
  table_oid_t TableOid() const { return table_oid_; }

  /** @return the child plan providing tuples to be inserted */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "update should have at most one child plan.");
    return GetChildAt(0);
  }

  const std::unordered_map<uint32_t, UpdateInfo> *GetUpdateAttr() const { return update_attrs_; }

 private:
  /** The table to be updated. */
  table_oid_t table_oid_;
  const std::unordered_map<uint32_t, UpdateInfo> *update_attrs_;
};
}  // namespace bustub
