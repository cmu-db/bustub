//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// mock_scan_executor.h
//
// Identification: src/include/execution/executors/mock_scan_executor.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include <string>
#include <vector>

#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/plans/mock_scan_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

extern const char *mock_table_list[];
auto GetMockTableSchemaOf(const std::string &table) -> Schema;

/**
 * The MockScanExecutor executor executes a sequential table scan for tests.
 */
class MockScanExecutor : public AbstractExecutor {
 public:
  MockScanExecutor(ExecutorContext *exec_ctx, const MockScanPlanNode *plan);

  void Init() override;
  auto Next(Tuple *tuple, RID *rid) -> bool override;

  /** @return The output schema for the sequential scan */
  auto GetOutputSchema() const -> const Schema & override { return plan_->OutputSchema(); }

 private:
  /** @return A dummy tuple according to the output schema */
  auto MakeDummyTuple() const -> Tuple;

  static auto MakeDummyRID() -> RID;

  /** MockScanExecutor::Next() returns `true` when scan is incomplete */
  constexpr static const bool EXECUTOR_ACTIVE{true};

  /** MockScanExecutor::Next() returns `false` when scan is complete */
  constexpr static const bool EXECUTOR_EXHAUSTED{false};

  /** The plan node for the scan */
  const MockScanPlanNode *plan_;

  /** The cursor for the current mock scan */
  std::size_t cursor_{0};

  /** The table function */
  std::function<Tuple(std::size_t)> func_;

  /** The size of the mock table */
  std::size_t size_;

  /** The shuffled output */
  std::vector<size_t> shuffled_idx_;
};

}  // namespace bustub
