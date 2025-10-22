//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// mock_scan_executor.cpp
//
// Identification: src/execution/mock_scan_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/mock_scan_executor.h"
#include <algorithm>
#include <random>

#include "common/exception.h"
#include "common/util/string_util.h"
#include "type/type_id.h"
#include "type/value_factory.h"

namespace bustub {

inline auto MockRandomValuesEnabled() -> bool {
  if (const char *v = std::getenv("BUSTUB_ENABLE_RANDOM"); (v != nullptr) && (*v != 0)) {
    return std::string_view(v) != "0";
  }
  return false;
}

inline auto MockSeed() -> uint64_t {
  if (const char *s = std::getenv("BUSTUB_MOCKSCAN_SEED"); (s != nullptr) && (*s != 0)) {
    try {
      return std::stoull(s);
    } catch (...) {
    }
  }
  if (const char *s = std::getenv("BUSTUB_TABLEGEN_SEED"); (s != nullptr) && (*s != 0)) {
    try {
      return std::stoull(s);
    } catch (...) {
    }
  }
  std::random_device rd;
  return (static_cast<uint64_t>(rd()) << 32) ^ static_cast<uint64_t>(rd());
}

// Per-table RNG: deterministic within a run, different across runs/seeds
inline auto MakeTableRng(const std::string &table) -> std::mt19937_64 {
  const uint64_t h = std::hash<std::string>{}(table);
  const uint64_t seed = MockSeed() ^ (h * 0x9E3779B97F4A7C15ULL);
  std::seed_seq seq{static_cast<uint32_t>(seed), static_cast<uint32_t>(seed >> 32), static_cast<uint32_t>(h),
                    static_cast<uint32_t>(h >> 32)};
  return std::mt19937_64(seq);
}

// Generic random value for a column (keeps types/sensible ranges)
inline auto RandomValueForColumn(const Column &col, std::mt19937_64 &rng) -> Value {
  switch (col.GetType()) {
    case TypeId::INTEGER: {
      // Keep modest range so joins can still occasionally match
      std::uniform_int_distribution<int32_t> d(0, 2000000);
      return ValueFactory::GetIntegerValue(d(rng));
    }
    case TypeId::VARCHAR: {
      // Length cap respects column length
      const uint32_t max_len = std::min<uint32_t>(col.GetStorageSize(), 32);
      std::uniform_int_distribution<int> len_d(0, static_cast<int>(max_len));
      const int len = len_d(rng);
      std::uniform_int_distribution<int> ch_d(33, 126);  // printable ASCII
      std::string s;
      s.reserve(len);
      for (int i = 0; i < len; i++) {
        s.push_back(static_cast<char>(ch_d(rng)));
      }
      return ValueFactory::GetVarcharValue(s);
    }
    default:
      return ValueFactory::GetZeroValueByType(col.GetType());
  }
}

static const char *ta_list_2022[] = {"amstqq",      "durovo",     "joyceliaoo", "karthik-ramanathan-3006",
                                     "kush789",     "lmwnshn",    "mkpjnx",     "skyzh",
                                     "thepinetree", "timlee0119", "yliang412"};

static const char *ta_list_2023[] = {"abigalekim",      "arvinwu168", "christopherlim98", "David-Lyons", "fanyuex2",
                                     "Mayank-Baranwal", "skyzh",      "yarkhinephyo",     "yliang412"};

static const char *ta_list_2023_fall[] = {"skyzh",     "yliang412",       "fernandolis10", "wiam8",
                                          "anurag-23", "Mayank-Baranwal", "abigalekim",    "ChaosZhai",
                                          "aoleizhou", "averyqi115",      "kswim8"};

static const char *ta_list_2024[] = {"AlSchlo",   "walkingcabbages", "averyqi115", "lanlou1554", "sweetsuro",
                                     "ChaosZhai", "SDTheSlayer",     "xx01cyx",    "yliang412",  "thelongmarch-azx"};

static const char *ta_list_2024_fall[] = {"17zhangw",         "connortsui20", "J-HowHuang", "lanlou1554",
                                          "prashanthduvvada", "unw9527",      "xx01cyx",    "yashkothari42"};

static const char *ta_list_2025_spring[] = {"AlSchlo",     "carpecodeum", "ChrisLaspias", "hyoungjook",
                                            "joesunil123", "mrwhitezz",   "rmboyce",      "yliang412"};

static const char *ta_list_2025_fall[] = {"17zhangw", "quantumish", "songwdfu", "notSaranshMalik",
                                          "shinyumh", "s-wangru",   "rayhhome", "MrWhitezz"};

static const char *ta_oh_2022[] = {"Tuesday",   "Wednesday", "Monday",  "Wednesday", "Thursday", "Friday",
                                   "Wednesday", "Randomly",  "Tuesday", "Monday",    "Tuesday"};

static const char *ta_oh_2023[] = {"Friday",  "Thursday", "Tuesday",   "Monday",  "Tuesday",
                                   "Tuesday", "Randomly", "Wednesday", "Thursday"};

static const char *ta_oh_2023_fall[] = {"Randomly", "Tuesday",   "Wednesday", "Tuesday", "Thursday", "Tuesday",
                                        "Friday",   "Yesterday", "Friday",    "Friday",  "Never"};

static const char *ta_oh_2024[] = {"Friday",    "Thursday", "Friday",  "Wednesday", "Thursday",
                                   "Yesterday", "Monday",   "Tuesday", "Tuesday",   "Monday"};

static const char *ta_oh_2024_fall[] = {"Wednesday", "Thursday", "Tuesday", "Monday",
                                        "Friday",    "Thursday", "Tuesday", "Friday"};

static const char *ta_oh_2025_spring[] = {"Friday", "Monday",   "Wednesday", "Tuesday",
                                          "Friday", "Thursday", "Monday",    "Tuesday"};

static const char *ta_oh_2025_fall[] = {"Tuesday", "Monday",  "Thursday", "Friday",
                                        "Tuesday", "Tuesday", "Friday",   "Wednesday"};

static const char *course_on_date[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

const char *mock_table_list[] = {"__mock_table_1", "__mock_table_2", "__mock_table_3", "__mock_table_4",
                                 "__mock_table_tas_2022", "__mock_table_tas_2023", "__mock_table_tas_2023_fall",
                                 "__mock_table_tas_2024", "__mock_table_tas_2024_fall", "__mock_table_tas_2025_spring",
                                 "__mock_table_tas_2025_fall", "__mock_agg_input_small", "__mock_agg_input_big",
                                 "__mock_external_merge_sort_input", "__mock_table_schedule_2022",
                                 "__mock_table_schedule", "__mock_table_123", "__mock_graph",
                                 // For leaderboard Q1
                                 "__mock_t1",
                                 // For leaderboard Q2
                                 "__mock_t4_1m", "__mock_t5_1m", "__mock_t6_1m",
                                 // For leaderboard Q3
                                 "__mock_t7", "__mock_t8", "__mock_t9",
                                 // For P3 leaderboard Q4
                                 "__mock_t10", "__mock_t11", nullptr};

static const int GRAPH_NODE_CNT = 10;

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

  if (table == "__mock_table_4") {
    return Schema{std::vector{Column{"colG", TypeId::INTEGER}, {Column{"colH", TypeId::VARCHAR, 128}}}};
  }

  if (table == "__mock_table_tas_2022") {
    return Schema{std::vector{Column{"github_id", TypeId::VARCHAR, 128}, Column{"office_hour", TypeId::VARCHAR, 128}}};
  }

  if (table == "__mock_table_tas_2023") {
    return Schema{std::vector{Column{"github_id", TypeId::VARCHAR, 128}, Column{"office_hour", TypeId::VARCHAR, 128}}};
  }

  if (table == "__mock_table_tas_2023_fall") {
    return Schema{std::vector{Column{"github_id", TypeId::VARCHAR, 128}, Column{"office_hour", TypeId::VARCHAR, 128}}};
  }

  if (table == "__mock_table_tas_2024") {
    return Schema{std::vector{Column{"github_id", TypeId::VARCHAR, 128}, Column{"office_hour", TypeId::VARCHAR, 128}}};
  }

  if (table == "__mock_table_tas_2024_fall") {
    return Schema{std::vector{Column{"github_id", TypeId::VARCHAR, 128}, Column{"office_hour", TypeId::VARCHAR, 128}}};
  }

  if (table == "__mock_table_tas_2025_spring") {
    return Schema{std::vector{Column{"github_id", TypeId::VARCHAR, 128}, Column{"office_hour", TypeId::VARCHAR, 128}}};
  }

  if (table == "__mock_table_tas_2025_fall") {
    return Schema{std::vector{Column{"github_id", TypeId::VARCHAR, 128}, Column{"office_hour", TypeId::VARCHAR, 128}}};
  }

  if (table == "__mock_table_schedule_2022") {
    return Schema{std::vector{Column{"day_of_week", TypeId::VARCHAR, 128}, Column{"has_lecture", TypeId::INTEGER}}};
  }

  if (table == "__mock_table_schedule") {
    return Schema{std::vector{Column{"day_of_week", TypeId::VARCHAR, 128}, Column{"has_lecture", TypeId::INTEGER}}};
  }

  if (table == "__mock_agg_input_small" || table == "__mock_agg_input_big") {
    return Schema{std::vector{Column{"v1", TypeId::INTEGER}, Column{"v2", TypeId::INTEGER},
                              Column{"v3", TypeId::INTEGER}, Column{"v4", TypeId::INTEGER},
                              Column{"v5", TypeId::INTEGER}, Column{"v6", TypeId::VARCHAR, 128}}};
  }

  if (table == "__mock_external_merge_sort_input") {
    return Schema{
        std::vector{Column{"v1", TypeId::INTEGER}, Column{"v2", TypeId::INTEGER}, Column{"v3", TypeId::INTEGER}}};
  }

  if (table == "__mock_graph") {
    // Note(f24): Make `src_label` and `dst_label` INTEGER because the current external merge sort implementation
    // only supports sorting on fixed-length data. Should be VARCHAR(8) if var-length data sorting is supported in
    // the future.
    return Schema{std::vector{Column{"src", TypeId::INTEGER}, Column{"dst", TypeId::INTEGER},
                              Column{"src_label", TypeId::INTEGER}, Column{"dst_label", TypeId::INTEGER},
                              Column{"distance", TypeId::INTEGER}}};
  }

  if (table == "__mock_table_123") {
    return Schema{std::vector{Column{"number", TypeId::INTEGER}}};
  }

  if (table == "__mock_t4_1m" || table == "__mock_t5_1m" || table == "__mock_t6_1m") {
    return Schema{std::vector{Column{"x", TypeId::INTEGER}, Column{"y", TypeId::INTEGER}}};
  }

  if (table == "__mock_t1") {
    return Schema{
        std::vector{Column{"x", TypeId::INTEGER}, Column{"y", TypeId::INTEGER}, Column{"z", TypeId::INTEGER}}};
  }

  if (table == "__mock_t7") {
    return Schema{
        std::vector{Column{"v", TypeId::INTEGER}, Column{"v1", TypeId::INTEGER}, Column{"v2", TypeId::INTEGER}}};
  }

  if (table == "__mock_t8") {
    return Schema{std::vector{Column{"v4", TypeId::INTEGER}}};
  }

  if (table == "__mock_t9") {
    return Schema{std::vector{Column{"x", TypeId::INTEGER}, Column{"y", TypeId::INTEGER}}};
  }

  if (table == "__mock_t10") {
    return Schema{std::vector{Column{"x", TypeId::INTEGER}, Column{"y", TypeId::INTEGER}}};
  }

  if (table == "__mock_t11") {
    return Schema{std::vector{Column{"x", TypeId::INTEGER}, Column{"y", TypeId::INTEGER}}};
  }

  throw bustub::Exception(fmt::format("mock table {} not found", table));
}

auto GetSizeOf(const MockScanPlanNode *plan) -> size_t {
  const auto &table = plan->GetTable();

  if (table == "__mock_table_1") {
    return 100;
  }

  if (table == "__mock_table_2") {
    return 100;
  }

  if (table == "__mock_table_3") {
    return 100;
  }

  if (table == "__mock_table_4") {
    return 100;
  }

  if (table == "__mock_table_tas_2022") {
    return sizeof(ta_list_2022) / sizeof(ta_list_2022[0]);
  }

  if (table == "__mock_table_tas_2023") {
    return sizeof(ta_list_2023) / sizeof(ta_list_2023[0]);
  }

  if (table == "__mock_table_tas_2023_fall") {
    return sizeof(ta_list_2023_fall) / sizeof(ta_list_2023_fall[0]);
  }

  if (table == "__mock_table_tas_2024") {
    return sizeof(ta_list_2024) / sizeof(ta_list_2024[0]);
  }

  if (table == "__mock_table_tas_2024_fall") {
    return sizeof(ta_list_2024_fall) / sizeof(ta_list_2024_fall[0]);
  }

  if (table == "__mock_table_tas_2025_spring") {
    return sizeof(ta_list_2025_spring) / sizeof(ta_list_2025_spring[0]);
  }

  if (table == "__mock_table_tas_2025_fall") {
    return sizeof(ta_list_2025_fall) / sizeof(ta_list_2025_fall[0]);
  }

  if (table == "__mock_table_schedule_2022") {
    return sizeof(course_on_date) / sizeof(course_on_date[0]);
  }

  if (table == "__mock_table_schedule") {
    return sizeof(course_on_date) / sizeof(course_on_date[0]);
  }

  if (table == "__mock_agg_input_small") {
    return 1000;
  }

  if (table == "__mock_agg_input_big") {
    return 10000;
  }

  if (table == "__mock_external_merge_sort_input") {
    return 100000;
  }

  if (table == "__mock_graph") {
    return GRAPH_NODE_CNT * GRAPH_NODE_CNT;
  }

  if (table == "__mock_table_123") {
    return 3;
  }

  if (table == "__mock_t1") {
    return 1000000;
  }

  if (table == "__mock_t4_1m" || table == "__mock_t5_1m" || table == "__mock_t6_1m") {
    return 1000000;
  }

  if (table == "__mock_t7") {
    return 1000000;
  }

  if (table == "__mock_t8") {
    return 10;
  }

  if (table == "__mock_t9") {
    return 10000000;
  }

  if (table == "__mock_t10") {
    return 10000;  // 10k
  }

  if (table == "__mock_t11") {
    return 1000000;  // 1M
  }

  return 0;
}

auto GetShuffled(const MockScanPlanNode *plan) -> bool {
  const auto &table = plan->GetTable();

  if (table == "__mock_t1") {
    return true;
  }

  if (table == "__mock_t2_100k") {
    return true;
  }

  if (table == "__mock_t3_1k") {
    return true;
  }

  return false;
}

auto GetFunctionOf(const MockScanPlanNode *plan) -> std::function<Tuple(size_t)> {
  const auto &table = plan->GetTable();
  if (table == "__mock_table_tas_2022") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(ta_list_2022[cursor]));
      values.push_back(ValueFactory::GetVarcharValue(ta_oh_2022[cursor]));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_tas_2023") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(ta_list_2023[cursor]));
      values.push_back(ValueFactory::GetVarcharValue(ta_oh_2023[cursor]));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_tas_2023_fall") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(ta_list_2023_fall[cursor]));
      values.push_back(ValueFactory::GetVarcharValue(ta_oh_2023_fall[cursor]));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_tas_2024") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(ta_list_2024[cursor]));
      values.push_back(ValueFactory::GetVarcharValue(ta_oh_2024[cursor]));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_tas_2024_fall") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(ta_list_2024_fall[cursor]));
      values.push_back(ValueFactory::GetVarcharValue(ta_oh_2024_fall[cursor]));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_tas_2025_spring") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(ta_list_2025_spring[cursor]));
      values.push_back(ValueFactory::GetVarcharValue(ta_oh_2025_spring[cursor]));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_tas_2025_fall") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(ta_list_2025_fall[cursor]));
      values.push_back(ValueFactory::GetVarcharValue(ta_oh_2025_fall[cursor]));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_schedule_2022") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(course_on_date[cursor]));
      values.push_back(ValueFactory::GetIntegerValue(cursor == 1 || cursor == 3 ? 1 : 0));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_schedule") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetVarcharValue(course_on_date[cursor]));
      values.push_back(ValueFactory::GetIntegerValue(cursor == 0 || cursor == 2 ? 1 : 0));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_1") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.reserve(2);
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue(cursor * 100));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_2") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.reserve(2);
      values.push_back(ValueFactory::GetVarcharValue(fmt::format("{}-\U0001F4A9", cursor)));  // the poop emoji
      values.push_back(
          ValueFactory::GetVarcharValue(StringUtil::Repeat("\U0001F607", cursor % 8)));  // the innocent emoji
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_3") {
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

  if (table == "__mock_table_4") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.reserve(2);
      if (cursor % 5 != 0) {
        values.push_back(ValueFactory::GetIntegerValue(cursor % 5));
      } else {
        values.push_back(ValueFactory::GetNullValueByType(TypeId::INTEGER));
      }

      std::string str = "\U0001F4A9";  // the poop emoji
      for (size_t i = 0; i < (cursor % 3); i++) {
        str += "\U0001F4A9";
      }
      if (cursor % 10 != 0) {
        values.push_back(ValueFactory::GetVarcharValue(str));
      } else {
        values.push_back(ValueFactory::GetNullValueByType(TypeId::VARCHAR));
      }

      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_agg_input_small") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue((cursor + 2) % 10));
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue((cursor + 50) % 100));
      values.push_back(ValueFactory::GetIntegerValue(cursor / 100));
      values.push_back(ValueFactory::GetIntegerValue(233));
      values.push_back(
          ValueFactory::GetVarcharValue(StringUtil::Repeat("\U0001F4A9", (cursor % 8) + 1)));  // the poop emoji
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_agg_input_big") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue((cursor + 2) % 10));
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue((cursor + 50) % 100));
      values.push_back(ValueFactory::GetIntegerValue(cursor / 1000));
      values.push_back(ValueFactory::GetIntegerValue(233));
      values.push_back(
          ValueFactory::GetVarcharValue(StringUtil::Repeat("\U0001F4A9", (cursor % 16) + 1)));  // the poop emoji
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_external_merge_sort_input") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue((cursor + 1777) % 15000));
      values.push_back(ValueFactory::GetIntegerValue((cursor + 3) % 111));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_table_123") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue(cursor + 1));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_graph") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      int src = cursor % GRAPH_NODE_CNT;
      int dst = cursor / GRAPH_NODE_CNT;
      values.push_back(ValueFactory::GetIntegerValue(src));
      values.push_back(ValueFactory::GetIntegerValue(dst));
      // Note(f24): Use INTEGER for `src_label` and `dst_label` based on current external merge sort.
      values.push_back(ValueFactory::GetIntegerValue(src * 100));
      values.push_back(ValueFactory::GetIntegerValue(dst * 100));
      if (src == dst) {
        values.push_back(ValueFactory::GetNullValueByType(TypeId::INTEGER));
      } else {
        values.push_back(ValueFactory::GetIntegerValue(1));
      }
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_t1") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue(cursor / 10000));
      values.push_back(ValueFactory::GetIntegerValue(cursor % 10000));
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_t8") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (MockRandomValuesEnabled() && table == "__mock_t7") {
    auto rng = MakeTableRng(table);
    const Schema *schema = &plan->OutputSchema();

    constexpr int32_t k_groups = 100;
    constexpr int32_t k_val_domain = 1000000;

    return [schema, rng](size_t /*cursor*/) mutable {
      auto v = static_cast<int32_t>(rng() % k_groups);
      auto v1 = static_cast<int32_t>(rng() % k_val_domain);
      auto v2 = static_cast<int32_t>(rng() % k_val_domain);
      std::vector<Value> values;
      values.emplace_back(ValueFactory::GetIntegerValue(v));
      values.emplace_back(ValueFactory::GetIntegerValue(v1));
      values.emplace_back(ValueFactory::GetIntegerValue(v2));
      return Tuple{values, schema};
    };
  }

  if (MockRandomValuesEnabled()) {
    auto rng = MakeTableRng(table);
    const Schema *schema = &plan->OutputSchema();
    return [schema, rng](size_t /*cursor*/) mutable {
      std::vector<Value> values;
      values.reserve(schema->GetColumnCount());
      for (const auto &col : schema->GetColumns()) {
        values.emplace_back(RandomValueForColumn(col, rng));
      }
      return Tuple{values, schema};
    };
  }

  if (table == "__mock_t4_1m") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      cursor = cursor % 500000;
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue(cursor * 10));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_t5_1m") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      cursor = (cursor + 30000) % 500000;
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue(cursor * 10));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_t6_1m") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      cursor = (cursor + 60000) % 500000;
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue(cursor * 10));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_t7") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue(cursor % 20));
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_t9") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue(cursor / 10000));
      values.push_back(
          ValueFactory::GetIntegerValue(10000000 - (cursor / 2 + ((cursor / 10000) % 2) * ((cursor / 2) % 2))));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_t10") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue(cursor));
      values.push_back(ValueFactory::GetIntegerValue(cursor * 10));
      return Tuple{values, &plan->OutputSchema()};
    };
  }

  if (table == "__mock_t11") {
    return [plan](size_t cursor) {
      std::vector<Value> values{};
      values.push_back(ValueFactory::GetIntegerValue(-1 * (cursor % 1000) - 1));
      values.push_back(ValueFactory::GetIntegerValue(cursor * 10));
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

/**
 * Construct a new MockScanExecutor instance.
 * @param exec_ctx The executor context
 * @param plan The mock scan plan to be executed
 */
MockScanExecutor::MockScanExecutor(ExecutorContext *exec_ctx, const MockScanPlanNode *plan)
    : AbstractExecutor{exec_ctx}, plan_{plan}, func_(GetFunctionOf(plan)), size_(GetSizeOf(plan)) {
  if (GetShuffled(plan)) {
    for (size_t i = 0; i < size_; i++) {
      shuffled_idx_.push_back(i);
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffled_idx_.begin(), shuffled_idx_.end(), g);
  }
}

/** Initialize the mock scan. */
void MockScanExecutor::Init() {
  // Reset the cursor
  cursor_ = 0;
}

/**
 * Yield the next tuple batch from the scan.
 * @param[out] tuple_batch The next tuple batch produced by the scan
 * @param[out] rid_batch The next tuple RID batch produced by the scan
 * @param batch_size The number of tuples to be included in the batch (default: BUSTUB_BATCH_SIZE)
 * @return `true` if a tuple was produced, `false` if there are no more tuples
 */
auto MockScanExecutor::Next(std::vector<bustub::Tuple> *tuple_batch, std::vector<bustub::RID> *rid_batch,
                            size_t batch_size) -> bool {
  tuple_batch->clear();
  rid_batch->clear();

  while (tuple_batch->size() < batch_size && cursor_ < size_) {
    Tuple tuple{};
    RID rid{};
    if (shuffled_idx_.empty()) {
      tuple = func_(cursor_);
    } else {
      tuple = func_(shuffled_idx_[cursor_]);
    }
    ++cursor_;
    rid = MakeDummyRID();

    tuple_batch->push_back(tuple);
    rid_batch->push_back(rid);
  }

  return !tuple_batch->empty();
}

/** @return A dummy RID value */
auto MockScanExecutor::MakeDummyRID() -> RID { return RID{0}; }

}  // namespace bustub
