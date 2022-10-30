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

#include "binder/table_ref/bound_base_table_ref.h"
#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * The SeqScanPlanNode represents a sequential table scan operation.
 */
class SeqScanPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new SeqScanPlanNode instance.
   * @param output The output schema of this sequential scan plan node
   * @param table_oid The identifier of table to be scanned
   */
  SeqScanPlanNode(SchemaRef output, table_oid_t table_oid, std::string table_name)
      : AbstractPlanNode(std::move(output), {}), table_oid_{table_oid}, table_name_(std::move(table_name)) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::SeqScan; }

  /** @return The identifier of the table that should be scanned */
  auto GetTableOid() const -> table_oid_t { return table_oid_; }

  static auto InferScanSchema(const BoundBaseTableRef &table_ref) -> Schema;

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(SeqScanPlanNode);

  /** The table whose tuples should be scanned */
  table_oid_t table_oid_;

  /** The table name */
  std::string table_name_;

 protected:
  auto PlanNodeToString() const -> std::string override { return fmt::format("SeqScan {{ table={} }}", table_name_); }
};

}  // namespace bustub
