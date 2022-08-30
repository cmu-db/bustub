#pragma once

#include <fmt/format.h>
#include <string>
#include <utility>
#include "../bound_table_ref.h"

namespace bustub {

/**
 * A bound table ref type for single table. e.g., `SELECT x FROM y`, where `y` is `BoundBaseTableRef`.
 */
class BoundBaseTableRef : public BoundTableRef {
 public:
  explicit BoundBaseTableRef(std::string table)
      : BoundTableRef(TableReferenceType::BASE_TABLE), table_(std::move(table)) {}

  auto ToString() const -> std::string override { return fmt::format("BoundBaseTableRef {{ table={} }}", table_); }

  // The name of the table
  std::string table_;
};
}  // namespace bustub
