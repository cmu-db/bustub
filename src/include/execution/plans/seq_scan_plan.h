//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// seq_scan_plan.h
//
// Identification: src/include/execution/plans/seq_scan_plan.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "catalog/simple_catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {
/**
 * SeqScanPlanNode identifies a table that should be scanned with an optional predicate.
 */
class SeqScanPlanNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new sequential scan plan node.
   * @param output the output format of this scan plan node
   * @param predicate the predicate to scan with, tuples are returned if predicate(tuple) = true or predicate = nullptr
   * @param table_oid the identifier of table to be scanned
   */
  SeqScanPlanNode(const Schema *output, const AbstractExpression *predicate, table_oid_t table_oid)
      : AbstractPlanNode(output, {}), predicate_{predicate}, table_oid_(table_oid) {}

  PlanType GetType() const override { return PlanType::SeqScan; }

  /** @return the predicate to test tuples against; tuples should only be returned if they evaluate to true */
  const AbstractExpression *GetPredicate() const { return predicate_; }

  /** @return the identifier of the table that should be scanned */
  table_oid_t GetTableOid() const { return table_oid_; }

 private:
  /** The predicate that all returned tuples must satisfy. */
  const AbstractExpression *predicate_;
  /** The table whose tuples should be scanned. */
  table_oid_t table_oid_;
};

}  // namespace bustub
