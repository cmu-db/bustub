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
#include "catalog/schema.h"
#include "execution/expressions/abstract_expression.h"
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
  NestedIndexJoinPlanNode(SchemaRef output, std::vector<AbstractPlanNodeRef> children, AbstractExpressionRef predicate,
                          table_oid_t inner_table_oid, std::string index_name, const Schema *outer_table_schema,
                          const Schema *inner_table_schema)
      : AbstractPlanNode(std::move(output), std::move(children)),
        predicate_(std::move(predicate)),
        inner_table_oid_(inner_table_oid),
        index_name_(std::move(index_name)),
        outer_table_schema_(outer_table_schema),
        inner_table_schema_(inner_table_schema) {}

  auto GetType() const -> PlanType override { return PlanType::NestedIndexJoin; }

  /** @return the predicate to be used in the nested index join */
  auto Predicate() const -> const AbstractExpressionRef & { return predicate_; }

  /** @return the plan node for the outer table of the nested index join */
  auto GetChildPlan() const -> AbstractPlanNodeRef { return GetChildAt(0); }

  /** @return the table oid for the inner table of the nested index join */
  auto GetInnerTableOid() const -> table_oid_t { return inner_table_oid_; }

  /** @return the index associated with the nested index join */
  auto GetIndexName() const -> std::string { return index_name_; }

  /** @return Schema with needed columns in from the outer table */
  auto OuterTableSchema() const -> const Schema & { return *outer_table_schema_; }

  /** @return Schema with needed columns in from the inner table */
  auto InnerTableSchema() const -> const Schema & { return *inner_table_schema_; }

 private:
  /** The nested index join predicate. */
  AbstractExpressionRef predicate_;
  table_oid_t inner_table_oid_;
  const std::string index_name_;
  SchemaRef outer_table_schema_;
  SchemaRef inner_table_schema_;
};
}  // namespace bustub
