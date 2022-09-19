#pragma once

#include <optional>
#include <string>
#include <utility>
#include "binder/bound_table_ref.h"
#include "catalog/schema.h"
#include "fmt/core.h"

namespace bustub {

/**
 * A bound table ref type for single table. e.g., `SELECT x FROM y`, where `y` is `BoundBaseTableRef`.
 */
class BoundBaseTableRef : public BoundTableRef {
 public:
  explicit BoundBaseTableRef(std::string table, std::optional<std::string> alias, Schema schema)
      : BoundTableRef(TableReferenceType::BASE_TABLE),
        table_(std::move(table)),
        alias_(std::move(alias)),
        schema_(std::move(schema)) {}

  auto ToString() const -> std::string override {
    if (alias_ == std::nullopt) {
      return fmt::format("BoundBaseTableRef {{ table={} }}", table_);
    }
    return fmt::format("BoundBaseTableRef {{ table={}, alias={} }}", table_, *alias_);
  }

  auto GetBoundTableName() const -> std::string {
    if (alias_ != std::nullopt) {
      return *alias_;
    }
    return table_;
  }

  /** The name of the table. */
  std::string table_;

  /** The alias of the table */
  std::optional<std::string> alias_;

  /** The schema of the table. */
  Schema schema_;
};
}  // namespace bustub
