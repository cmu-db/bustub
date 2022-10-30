//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// mock_scan_plan.h
//
// Identification: src/include/execution/plans/mock_scan_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <utility>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * The MockScanPlanNode represents a "dummy" sequential
 * scan over a table, without requiring the table to exist.
 * NOTE: This class is used solely for testing.
 */
class MockScanPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new MockScanPlanNode instance.
   * @param output The output schema of this mock scan plan node
   */
  MockScanPlanNode(SchemaRef output, std::string table)
      : AbstractPlanNode(std::move(output), {}), table_(std::move(table)) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::MockScan; }

  /** @return The table name of this mock scan node, used to determine the generated content. */
  auto GetTable() const -> const std::string & { return table_; }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(MockScanPlanNode);

 protected:
  auto PlanNodeToString() const -> std::string override { return fmt::format("MockScan {{ table={} }}", table_); }

 private:
  /** The table name of this mock scan executor */
  std::string table_;
};

}  // namespace bustub
