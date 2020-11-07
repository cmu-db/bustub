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

#include "catalog/catalog.h"
#include "execution/plans/abstract_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * NestedIndexJoinPlanNode is used to represent performing a nested index join between two tables
 * The outer table tuples are propogated using a child executor, but the inner table tuples should be
 * obtained using the outer table tuples as well as the index from the catalog.
 */
class NestedIndexJoinPlanNode : public AbstractPlanNode {
 public:
  NestedIndexJoinPlanNode(const Schema *output_schema, std::vector<const AbstractPlanNode *> &&children,
                          const AbstractExpression *predicate, table_oid_t inner_table_oid, std::string index_name,
                          const Schema *outer_table_schema, const Schema *inner_table_schema)
      : AbstractPlanNode(output_schema, std::move(children)),
        predicate_(predicate),
        inner_table_oid_(inner_table_oid),
        index_name_(std::move(index_name)),
        outer_table_schema_(outer_table_schema),
        inner_table_schema_(inner_table_schema) {}

  PlanType GetType() const override { return PlanType::NestedIndexJoin; }

  /** @return the predicate to be used in the nested index join */
  const AbstractExpression *Predicate() const { return predicate_; }

  /** @return the plan node for the outer table of the nested index join */
  const AbstractPlanNode *GetChildPlan() const { return GetChildAt(0); }

  /** @return the table oid for the inner table of the nested index join */
  table_oid_t GetInnerTableOid() const { return inner_table_oid_; }

  /** @return the index associated with the nested index join */
  std::string GetIndexName() const { return index_name_; }

  /** @return Schema with needed columns in from the outer table */
  const Schema *OuterTableSchema() const { return outer_table_schema_; }

  /** @return Schema with needed columns in from the inner table */
  const Schema *InnerTableSchema() const { return inner_table_schema_; }

 private:
  /** The nested index join predicate. */
  const AbstractExpression *predicate_;
  table_oid_t inner_table_oid_;
  const std::string index_name_;
  const Schema *outer_table_schema_;
  const Schema *inner_table_schema_;
};
}  // namespace bustub
