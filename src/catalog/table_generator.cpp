#include "catalog/table_generator.h"

#include <algorithm>
#include <random>
#include <vector>

namespace bustub {

template <typename CppType>
std::vector<Value> TableGenerator::GenNumericValues(ColumnInsertMeta *col_meta, uint32_t count) {
  std::vector<Value> values;
  if (col_meta->dist_ == Dist::Serial) {
    for (uint32_t i = 0; i < count; i++) {
      values.emplace_back(Value(col_meta->type_, static_cast<CppType>(col_meta->serial_counter_)));
      col_meta->serial_counter_ += 1;
    }
    return values;
  }
  std::default_random_engine generator;
  // TODO(Amadou): Break up in two branches if this is too weird.
  std::conditional_t<std::is_integral_v<CppType>, std::uniform_int_distribution<CppType>,
                     std::uniform_real_distribution<CppType>>
      distribution(static_cast<CppType>(col_meta->min_), static_cast<CppType>(col_meta->max_));
  for (uint32_t i = 0; i < count; i++) {
    values.emplace_back(Value(col_meta->type_, distribution(generator)));
  }
  return values;
}

std::vector<Value> TableGenerator::MakeValues(ColumnInsertMeta *col_meta, uint32_t count) {
  std::vector<Value> values;
  switch (col_meta->type_) {
    case TypeId::TINYINT:
      return GenNumericValues<int8_t>(col_meta, count);
    case TypeId::SMALLINT:
      return GenNumericValues<int16_t>(col_meta, count);
    case TypeId::INTEGER:
      return GenNumericValues<int32_t>(col_meta, count);
    case TypeId::BIGINT:
      return GenNumericValues<int64_t>(col_meta, count);
    case TypeId::DECIMAL:
      return GenNumericValues<double>(col_meta, count);
    default:
      UNREACHABLE("Not yet implemented");
  }
}

void TableGenerator::FillTable(TableMetadata *info, TableInsertMeta *table_meta) {
  uint32_t num_inserted = 0;
  uint32_t batch_size = 128;
  while (num_inserted < table_meta->num_rows_) {
    std::vector<std::vector<Value>> values;
    uint32_t num_values = std::min(batch_size, table_meta->num_rows_ - num_inserted);
    for (auto &col_meta : table_meta->col_meta_) {
      values.emplace_back(MakeValues(&col_meta, num_values));
    }
    for (uint32_t i = 0; i < num_values; i++) {
      std::vector<Value> entry;
      entry.reserve(values.size());
      for (const auto &col : values) {
        entry.emplace_back(col[i]);
      }
      RID rid;
      bool inserted = info->table_->InsertTuple(Tuple(entry, &info->schema_), &rid, exec_ctx_->GetTransaction());
      BUSTUB_ASSERT(inserted, "Sequential insertion cannot fail");
      num_inserted++;
    }
    // exec_ctx_->GetBufferPoolManager()->FlushAllPages();
  }
  LOG_INFO("Wrote %d tuples to table %s.", num_inserted, table_meta->name_);
}

void TableGenerator::GenerateTestTables() {
  /**
   * This array configures each of the test tables. Each table is configured
   * with a name, size, and schema. We also configure the columns of the table. If
   * you add a new table, set it up here.
   */
  std::vector<TableInsertMeta> insert_meta{
      // The empty table
      {"empty_table", 0, {{"colA", TypeId::INTEGER, false, Dist::Serial, 0, 0}}},

      // Table 1
      {"test_1",
       TEST1_SIZE,
       {{"colA", TypeId::INTEGER, false, Dist::Serial, 0, 0},
        {"colB", TypeId::INTEGER, false, Dist::Uniform, 0, 9},
        {"colC", TypeId::INTEGER, false, Dist::Uniform, 0, 9999},
        {"colD", TypeId::INTEGER, false, Dist::Uniform, 0, 99999}}},

      // Table 2
      {"test_2",
       TEST2_SIZE,
       {{"col1", TypeId::SMALLINT, false, Dist::Serial, 0, 0},
        {"col2", TypeId::INTEGER, true, Dist::Uniform, 0, 9},
        {"col3", TypeId::BIGINT, false, Dist::Uniform, 0, 1024},
        {"col4", TypeId::INTEGER, true, Dist::Uniform, 0, 2048}}},

      // Table 3
      {"test_3",
       TEST2_SIZE,
       {{"col1", TypeId::INTEGER, false, Dist::Serial, 0, 0},
        {"col2", TypeId::INTEGER, true, Dist::Uniform, 10, 19},
        {"col3", TypeId::BIGINT, false, Dist::Uniform, 0, 1024},
        {"col4", TypeId::INTEGER, true, Dist::Uniform, 0, 2048}}},

      // Empty table with two columns
      {"empty_table2",
       0,
       {{"colA", TypeId::INTEGER, false, Dist::Serial, 0, 0}, {"colB", TypeId::INTEGER, false, Dist::Uniform, 0, 9}}},

      // Empty table with two columns
      {"empty_table3",
       0,
       {{"outA", TypeId::INTEGER, false, Dist::Serial, 0, 0}, {"outB", TypeId::INTEGER, false, Dist::Uniform, 0, 9}}},
  };

  for (auto &table_meta : insert_meta) {
    // Create Schema
    std::vector<Column> cols{};
    cols.reserve(table_meta.col_meta_.size());
    for (const auto &col_meta : table_meta.col_meta_) {
      if (col_meta.type_ != TypeId::VARCHAR) {
        cols.emplace_back(col_meta.name_, col_meta.type_);
      } else {
        cols.emplace_back(col_meta.name_, col_meta.type_, TEST_VARLEN_SIZE);
      }
    }
    Schema schema(cols);
    auto info = exec_ctx_->GetCatalog()->CreateTable(exec_ctx_->GetTransaction(), table_meta.name_, schema);
    FillTable(info, &table_meta);
  }
}
}  // namespace bustub
