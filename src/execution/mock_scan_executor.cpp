//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// mock_scan_executor.cpp
//
// Identification: src/execution/mock_scan_executor.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/mock_scan_executor.h"

#include "common/util/string_util.h"
#include "execution/expressions/column_value_expression.h"
#include "type/type_id.h"
#include "type/value_factory.h"

namespace bustub {

MockScanExecutor::MockScanExecutor(ExecutorContext *exec_ctx, const MockScanPlanNode *plan)
    : AbstractExecutor{exec_ctx}, plan_{plan} {}

void MockScanExecutor::Init() {
  // Reset the cursor
  cursor_ = 0;
}

auto MockScanExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  if (cursor_ == plan_->GetSize()) {
    // Scan complete
    return EXECUTOR_EXHAUSTED;
  }
  ++cursor_;
  *tuple = MakeDummyTuple();
  *rid = MakeDummyRID();
  plan_->IncrementPollCount();
  return EXECUTOR_ACTIVE;
}

/** @return A dummy tuple according to the output schema */
auto MockScanExecutor::MakeDummyTuple() const -> Tuple {
  if (plan_->GetTable() == "__mock_table_1") {
    std::vector<Value> values{};
    values.reserve(2);
    values.push_back(ValueFactory::GetIntegerValue(cursor_));
    values.push_back(ValueFactory::GetIntegerValue(cursor_ * 100));
    return Tuple{values, GetOutputSchema()};
  }
  if (plan_->GetTable() == "__mock_table_2") {
    std::vector<Value> values{};
    values.reserve(2);
    values.push_back(ValueFactory::GetVarcharValue(fmt::format("{}-\U0001F4A9", cursor_)));  // the poop emoji
    values.push_back(
        ValueFactory::GetVarcharValue(StringUtil::Repeat("\U0001F607", cursor_ % 8)));  // the innocent emoji
    return Tuple{values, GetOutputSchema()};
  }
  if (plan_->GetTable() == "__mock_table_3") {
    std::vector<Value> values{};
    values.reserve(2);
    if (cursor_ % 2 == 0) {
      values.push_back(ValueFactory::GetIntegerValue(cursor_));
    } else {
      values.push_back(ValueFactory::GetNullValueByType(TypeId::INTEGER));
    }
    values.push_back(ValueFactory::GetVarcharValue(fmt::format("{}-\U0001F4A9", cursor_)));  // the poop emoji
    return Tuple{values, GetOutputSchema()};
  }
  std::vector<Value> values{};
  values.reserve(GetOutputSchema()->GetColumnCount());
  for (const auto &column : GetOutputSchema()->GetColumns()) {
    values.push_back(ValueFactory::GetZeroValueByType(column.GetType()));
  }
  return Tuple{values, GetOutputSchema()};
}

auto MockScanExecutor::MakeDummyRID() -> RID { return RID{0}; }

}  // namespace bustub
