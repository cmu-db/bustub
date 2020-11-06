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
                          const AbstractExpression *predicate, const AbstractExpression *inner_predicate,
                          table_oid_t inner_table_oid, std::string index_name)
      : AbstractPlanNode(output_schema, std::move(children)),
        predicate_(predicate),
        inner_predicate_(inner_predicate),
        inner_table_oid_(inner_table_oid),
        index_name_(std::move(index_name)) {}

  PlanType GetType() const override { return PlanType::NestedIndexJoin; }

  /** @return the predicate to be used in the nested index join */
  const AbstractExpression *Predicate() const { return predicate_; }

  /** @return the predicate for the inner table to be used in the nested index join */
  const AbstractExpression *Inner_Predicate() const { return inner_predicate_; }

  /** @return the plan node for the outer table of the nested index join */
  const AbstractPlanNode *GetChildPlan() const { return GetChildAt(0); }

  table_oid_t GetInnerTableOid() const { return inner_table_oid_; }

  const std::string &GetIndexName() const { return index_name_; }

 private:
  /** The nested index join predicate. */
  const AbstractExpression *predicate_;
  const AbstractExpression *inner_predicate_;
  table_oid_t inner_table_oid_;
  const std::string index_name_;
};
}  // namespace bustub
