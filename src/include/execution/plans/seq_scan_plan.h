//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// seq_scan_plan.h
//
// Identification: src/include/execution/plans/seq_scan_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <utility>

#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * The SeqScanPlanNode represents a sequential table scan operation.
 * It identifies a table to be scanned and an optional predicate.
 */
class SeqScanPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new SeqScanPlanNode instance.
   * @param output The output schema of this sequential scan plan node
   * @param predicate The predicate applied during the scan operation
   * @param table_oid The identifier of table to be scanned
   */
  SeqScanPlanNode(SchemaRef output, AbstractExpressionRef predicate, table_oid_t table_oid)
      : AbstractPlanNode(std::move(output), {}), predicate_(std::move(predicate)), table_oid_{table_oid} {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::SeqScan; }

  /** @return The predicate to test tuples against; tuples should only be returned if they evaluate to true */
  auto GetPredicate() const -> AbstractExpressionRef { return predicate_; }

  /** @return The identifier of the table that should be scanned */
  auto GetTableOid() const -> table_oid_t { return table_oid_; }

  static auto InferScanSchema(const TableInfo &table_info) -> Schema;

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(SeqScanPlanNode);

 protected:
  auto PlanNodeToString() const -> std::string override {
    return fmt::format("SeqScan {{ table_oid={} }}", table_oid_);
  }

 private:
  /** The predicate that all returned tuples must satisfy */
  AbstractExpressionRef predicate_;
  /** The table whose tuples should be scanned */
  table_oid_t table_oid_;
};

}  // namespace bustub
