//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// nested_index_join_plan.h
//
// Identification: src/include/execution/plans/nested_index_join_plan.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "binder/table_ref/bound_join_ref.h"
#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "concurrency/transaction.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * NestedIndexJoinPlanNode is used to represent performing a nested index join between two tables
 * The outer table tuples are propagated using a child executor, but the inner table tuples should be
 * obtained using the outer table tuples as well as the index from the catalog.
 */
class NestedIndexJoinPlanNode : public AbstractPlanNode {
 public:
  NestedIndexJoinPlanNode(SchemaRef output, AbstractPlanNodeRef child, AbstractExpressionRef key_predicate,
                          table_oid_t inner_table_oid, index_oid_t index_oid, std::string index_name,
                          std::string index_table_name, SchemaRef inner_table_schema, JoinType join_type)
      : AbstractPlanNode(std::move(output), {std::move(child)}),
        key_predicate_(std::move(key_predicate)),
        inner_table_oid_(inner_table_oid),
        index_oid_(index_oid),
        index_name_(std::move(index_name)),
        index_table_name_(std::move(index_table_name)),
        inner_table_schema_(std::move(inner_table_schema)),
        join_type_(join_type) {}

  auto GetType() const -> PlanType override { return PlanType::NestedIndexJoin; }

  /** @return The predicate to be used to extract the join key from the child */
  auto KeyPredicate() const -> const AbstractExpressionRef & { return key_predicate_; }

  /** @return The join type used in the nested index join */
  auto GetJoinType() const -> JoinType { return join_type_; };

  /** @return The plan node for the outer table of the nested index join */
  auto GetChildPlan() const -> AbstractPlanNodeRef { return GetChildAt(0); }

  /** @return The table oid for the inner table of the nested index join */
  auto GetInnerTableOid() const -> table_oid_t { return inner_table_oid_; }

  /** @return The index associated with the nested index join */
  auto GetIndexName() const -> std::string { return index_name_; }

  /** @return The index oid associated with the nested index join */
  auto GetIndexOid() const -> index_oid_t { return index_oid_; }

  /** @return Schema with needed columns in from the inner table */
  auto InnerTableSchema() const -> const Schema & { return *inner_table_schema_; }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(NestedIndexJoinPlanNode);

  /** The nested index join predicate. */
  AbstractExpressionRef key_predicate_;
  table_oid_t inner_table_oid_;
  index_oid_t index_oid_;
  const std::string index_name_;
  const std::string index_table_name_;
  SchemaRef inner_table_schema_;

  /** The join type */
  JoinType join_type_;

 protected:
  auto PlanNodeToString() const -> std::string override {
    return fmt::format("NestedIndexJoin {{ type={}, key_predicate={}, index={}, index_table={} }}", join_type_,
                       key_predicate_, index_name_, index_table_name_);
  }
};
}  // namespace bustub
