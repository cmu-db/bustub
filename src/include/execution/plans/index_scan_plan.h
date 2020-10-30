//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// index_scan_plan.h
//
// Identification: src/include/execution/plans/index_scan_plan.h
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
 * IndexScanPlanNode identifies a table that should be scanned with an optional predicate.
 */
class IndexScanPlanNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new index scan plan node.
   * @param output the output format of this scan plan node
   * @param predicate the predicate to scan with, tuples are returned if predicate(tuple) == true or predicate ==
   * nullptr
   * @param table_oid the identifier of table to be scanned
   */
  IndexScanPlanNode(const Schema *output, const AbstractExpression *predicate, index_oid_t index_oid)
      : AbstractPlanNode(output, {}), predicate_{predicate}, index_oid_(index_oid) {}

  PlanType GetType() const override { return PlanType::IndexScan; }

  /** @return the predicate to test tuples against; tuples should only be returned if they evaluate to true */
  const AbstractExpression *GetPredicate() const { return predicate_; }

  /** @return the identifier of the table that should be scanned */
  index_oid_t GetIndexOid() const { return index_oid_; }

 private:
  /** The predicate that all returned tuples must satisfy. */
  const AbstractExpression *predicate_;
  /** The table whose tuples should be scanned. */
  index_oid_t index_oid_;
};

}  // namespace bustub
