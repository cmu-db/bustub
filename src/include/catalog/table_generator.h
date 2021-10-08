#pragma once

#include <utility>
#include <vector>

#include "catalog/catalog.h"
#include "execution/executor_context.h"
#include "storage/table/table_heap.h"

namespace bustub {

static constexpr uint32_t TEST1_SIZE = 1000;
static constexpr uint32_t TEST2_SIZE = 100;
static constexpr uint32_t TEST3_SIZE = 100;
static constexpr uint32_t TEST4_SIZE = 100;
static constexpr uint32_t TEST6_SIZE = 100;
static constexpr uint32_t TEST7_SIZE = 100;
static constexpr uint32_t TEST8_SIZE = 10;
static constexpr uint32_t TEST9_SIZE = 10;
static constexpr uint32_t TEST_VARLEN_SIZE = 10;

class TableGenerator {
 public:
  /**
   * Constructor
   */
  explicit TableGenerator(ExecutorContext *exec_ctx) : exec_ctx_{exec_ctx} {}

  /**
   * Generate test tables.
   */
  void GenerateTestTables();

 private:
  /** Enumeration to characterize the distribution of values in a given column */
  enum class Dist : uint8_t { Uniform, Zipf_50, Zipf_75, Zipf_95, Zipf_99, Serial, Cyclic };

  /**
   * Metadata about the data for a given column. Specifically, the type of the
   * column, the distribution of values, a min and max if appropriate.
   */
  struct ColumnInsertMeta {
    /**
     * Name of the column
     */
    const char *name_;
    /**
     * Type of the column
     */
    const TypeId type_;
    /**
     * Whether the column is nullable
     */
    bool nullable_;
    /**
     * Distribution of values
     */
    Dist dist_;
    /**
     * Min value of the column
     */
    uint64_t min_;
    /**
     * Max value of the column
     */
    uint64_t max_;
    /**
     * Counter to generate serial data
     */
    uint64_t serial_counter_{0};

    /**
     * Constructor
     */
    ColumnInsertMeta(const char *name, const TypeId type, bool nullable, Dist dist, uint64_t min, uint64_t max)
        : name_(name), type_(type), nullable_(nullable), dist_(dist), min_(min), max_(max) {}
  };

  /**
   * Metadata about a table. Specifically, the schema and number of
   * rows in the table.
   */
  struct TableInsertMeta {
    /**
     * Name of the table
     */
    const char *name_;
    /**
     * Number of rows
     */
    uint32_t num_rows_;
    /**
     * Columns
     */
    std::vector<ColumnInsertMeta> col_meta_;

    /**
     * Constructor
     */
    TableInsertMeta(const char *name, uint32_t num_rows, std::vector<ColumnInsertMeta> col_meta)
        : name_(name), num_rows_(num_rows), col_meta_(std::move(col_meta)) {}
  };

  void FillTable(TableInfo *info, TableInsertMeta *table_meta);

  std::vector<Value> MakeValues(ColumnInsertMeta *col_meta, uint32_t count);

  template <typename CppType>
  std::vector<Value> GenNumericValues(ColumnInsertMeta *col_meta, uint32_t count);

 private:
  ExecutorContext *exec_ctx_;
};
}  // namespace bustub
