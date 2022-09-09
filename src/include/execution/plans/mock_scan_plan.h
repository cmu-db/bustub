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
   * @param size The size of the scanned table
   */
  MockScanPlanNode(const Schema *output, std::size_t size) : AbstractPlanNode(output, {}), size_{size} {}

  /**
   * Construct a new MockScanPlanNode instance.
   * @param output The output schema of this mock scan plan node
   * @param size The size of the scanned table
   * @param table The table name of this mock scan node, used to determine the generated content.
   */
  MockScanPlanNode(const Schema *output, std::size_t size, std::string table)
      : AbstractPlanNode(output, {}), size_{size}, table_(std::move(table)) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::MockScan; }

  /** @return The size of this "mock" scan */
  auto GetSize() const -> std::size_t { return size_; }

  /** @return The total number of times the executor for this plan is polled */
  auto PollCount() const -> std::size_t { return poll_count_; }

  /** Increment the poll count for this mock scan */
  void IncrementPollCount() const { poll_count_++; }

  /** @return The table name of this mock scan node, used to determine the generated content. */
  auto GetTable() const -> const std::string & { return table_; }

 protected:
  auto PlanNodeToString() const -> std::string override { return fmt::format("MockScan {{ size={} }}", size_); }

 private:
  /** The size of the scanned table */
  const std::size_t size_;
  /** The total number of times the executor for this plan is polled */
  mutable std::size_t poll_count_{0};
  /** The table name of this mock scan executor */
  std::string table_;
};

}  // namespace bustub
