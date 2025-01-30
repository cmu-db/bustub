//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// schema.h
//
// Identification: src/include/catalog/schema.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "catalog/column.h"
#include "common/exception.h"
#include "type/type.h"

namespace bustub {

class Schema;
using SchemaRef = std::shared_ptr<const Schema>;

class Schema {
 public:
  explicit Schema(const std::vector<Column> &columns);

  static auto CopySchema(const Schema *from, const std::vector<uint32_t> &attrs) -> Schema {
    std::vector<Column> cols;
    cols.reserve(attrs.size());
    for (const auto i : attrs) {
      cols.emplace_back(from->columns_[i]);
    }
    return Schema{cols};
  }

  /** @return all the columns in the schema */
  auto GetColumns() const -> const std::vector<Column> & { return columns_; }

  /**
   * Returns a specific column from the schema.
   * @param col_idx index of requested column
   * @return requested column
   */
  auto GetColumn(const uint32_t col_idx) const -> const Column & { return columns_[col_idx]; }

  /**
   * Looks up and returns the index of the first column in the schema with the specified name.
   * If multiple columns have the same name, the first such index is returned.
   * @param col_name name of column to look for
   * @return the index of a column with the given name, throws an exception if it does not exist
   */
  auto GetColIdx(const std::string &col_name) const -> uint32_t {
    if (auto col_idx = TryGetColIdx(col_name)) {
      return *col_idx;
    }
    UNREACHABLE("Column does not exist");
  }

  /**
   * Looks up and returns the index of the first column in the schema with the specified name.
   * If multiple columns have the same name, the first such index is returned.
   * @param col_name name of column to look for
   * @return the index of a column with the given name, `std::nullopt` if it does not exist
   */
  auto TryGetColIdx(const std::string &col_name) const -> std::optional<uint32_t> {
    for (uint32_t i = 0; i < columns_.size(); ++i) {
      if (columns_[i].GetName() == col_name) {
        return std::optional{i};
      }
    }
    return std::nullopt;
  }

  /** @return the indices of non-inlined columns */
  auto GetUnlinedColumns() const -> const std::vector<uint32_t> & { return uninlined_columns_; }

  /** @return the number of columns in the schema for the tuple */
  auto GetColumnCount() const -> uint32_t { return static_cast<uint32_t>(columns_.size()); }

  /** @return the number of non-inlined columns */
  auto GetUnlinedColumnCount() const -> uint32_t { return static_cast<uint32_t>(uninlined_columns_.size()); }

  /** @return the number of bytes used by one tuple */
  inline auto GetInlinedStorageSize() const -> uint32_t { return length_; }

  /** @return true if all columns are inlined, false otherwise */
  inline auto IsInlined() const -> bool { return tuple_is_inlined_; }

  auto ToString(bool simplified = true) const -> std::string;

 private:
  /** Fixed-length column size, i.e. the number of bytes used by one tuple. */
  uint32_t length_;

  /** All the columns in the schema, inlined and uninlined. */
  std::vector<Column> columns_;

  /** True if all the columns are inlined, false otherwise. */
  bool tuple_is_inlined_{true};

  /** Indices of all uninlined columns. */
  std::vector<uint32_t> uninlined_columns_;
};

}  // namespace bustub

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<bustub::Schema, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const bustub::Schema &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<std::shared_ptr<T>, std::enable_if_t<std::is_base_of<bustub::Schema, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::shared_ptr<T> &x, FormatCtx &ctx) const {
    if (x != nullptr) {
      return fmt::formatter<std::string>::format(x->ToString(), ctx);
    }
    return fmt::formatter<std::string>::format("", ctx);
  }
};

template <typename T>
struct fmt::formatter<std::unique_ptr<T>, std::enable_if_t<std::is_base_of<bustub::Schema, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<T> &x, FormatCtx &ctx) const {
    if (x != nullptr) {
      return fmt::formatter<std::string>::format(x->ToString(), ctx);
    }
    return fmt::formatter<std::string>::format("", ctx);
  }
};
