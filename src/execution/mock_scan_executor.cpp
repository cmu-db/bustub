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

#include "common/exception.h"
#include "common/util/string_util.h"
#include "execution/expressions/column_value_expression.h"
#include "type/type_id.h"
#include "type/value_factory.h"

namespace bustub {

const char *ta_list_2022[] = {"amstqq",      "durovo",     "joyceliaoo", "karthik-ramanathan-3006",
                              "kush789",     "lmwnshn",    "mkpjnx",     "skyzh",
                              "thepinetree", "timlee0119", "yliang412"};

const char *mock_table_list[] = {"__mock_table_1", "__mock_table_2", "__mock_table_3", "__mock_table_tas_2022",
                                 nullptr};

auto GetMockTableSchemaOf(const std::string &table) -> Schema {
  if (table == "__mock_table_1") {
    return Schema{std::vector{{Column{"colA", TypeId::INTEGER}, {Column{"colB", TypeId::INTEGER}}}}};
  }

  if (table == "__mock_table_2") {
    return Schema{std::vector{Column{"colC", TypeId::VARCHAR, 128}, {Column{"colD", TypeId::VARCHAR, 128}}}};
  }

  if (table == "__mock_table_3") {
    return Schema{std::vector{Column{"colE", TypeId::INTEGER}, {Column{"colF", TypeId::VARCHAR, 128}}}};
  }

  if (table == "__mock_table_tas_2022") {
    return Schema{std::vector{Column{"github_id", TypeId::VARCHAR, 128}}};
  }

  throw bustub::Exception(fmt::format("mock table {} not found", table));
}

auto GetSizeOf(const MockScanPlanNode *plan) -> size_t {
  if (plan->GetTable() == "__mock_table_1") {
    return 100;
  }

  if (plan->GetTable() == "__mock_table_2") {
    return 100;
  }

  if (plan->GetTable() == "__mock_table_3") {
    return 100;
  }

  if (plan->GetTable() == "__mock_table_tas_2022") {
    return sizeof(ta_list_2022) / sizeof(ta_list_2022[0]);
  }

  return 100;
}

auto GetFunctionOf(const MockScanPlanNode *plan) -> std::function<Tuple(size_t)> {
  if (plan->GetTable() == "__mock_table_1") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.reserve(2);
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue(cursor * 100));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (plan->GetTable() == "__mock_table_2") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.reserve(2);
      values.push_back(ValueFactory::GetVarcharValue(fmt::format("{}-\U0001F4A9", cursor)));  // the poop emoji
      values.push_back(
          ValueFactory::GetVarcharValue(StringUtil::Repeat("\U0001F607", cursor % 8)));  // the innocent emoji
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (plan->GetTable() == "__mock_table_3") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.reserve(2);
      if (cursor % 2 == 0) {
        values.push_back(ValueFactory::GetIntegerValue(cursor));
      } else {
        values.push_back(ValueFactory::GetNullValueByType(TypeId::INTEGER));
      }
      values.push_back(ValueFactory::GetVarcharValue(fmt::format("{}-\U0001F4A9", cursor)));  // the poop emoji
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (plan->GetTable() == "__mock_table_tas_2022") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(ta_list_2022[cursor]));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  // By default, return table of all 0.
  return [plan](size_t cursor) {
    std::vector<Value> values{};
    values.reserve(plan->OutputSchema().GetColumnCount());
    for (const auto &column : plan->OutputSchema().GetColumns()) {
      values.push_back(ValueFactory::GetZeroValueByType(column.GetType()));
    }
    return Tuple{values, &plan->OutputSchema()};
  };
}

MockScanExecutor::MockScanExecutor(ExecutorContext *exec_ctx, const MockScanPlanNode *plan)
    : AbstractExecutor{exec_ctx}, plan_{plan}, func_(GetFunctionOf(plan)), size_(GetSizeOf(plan)) {}

void MockScanExecutor::Init() {
  // Reset the cursor
  cursor_ = 0;
}

auto MockScanExecutor::Next(Tuple *tuple, RID *rid) -> bool {
  if (cursor_ == size_) {
    // Scan complete
    return EXECUTOR_EXHAUSTED;
  }
  *tuple = func_(cursor_);
  ++cursor_;
  *rid = MakeDummyRID();
  return EXECUTOR_ACTIVE;
}

auto MockScanExecutor::MakeDummyRID() -> RID { return RID{0}; }

}  // namespace bustub
