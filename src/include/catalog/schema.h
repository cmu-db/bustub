//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// schema.h
//
// Identification: src/include/catalog/schema.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "catalog/column.h"
#include "type/type.h"

namespace bustub {

class Schema {
 public:
  /**
   * Constructs the schema corresponding to the vector of columns, read left-to-right.
   * @param columns columns that describe the schema's individual columns
   */
  explicit Schema(const std::vector<Column> &columns);

  static Schema *CopySchema(const Schema *from, const std::vector<uint32_t> &attrs) {
    std::vector<Column> cols;
    cols.reserve(attrs.size());
    for (const auto i : attrs) {
      cols.emplace_back(from->columns_[i]);
    }
    return new Schema{cols};
  }

  /** @return all the columns in the schema */
  const std::vector<Column> &GetColumns() const { return columns_; }

  /**
   * Returns a specific column from the schema.
   * @param col_idx index of requested column
   * @return requested column
   */
  const Column &GetColumn(const uint32_t col_idx) const { return columns_[col_idx]; }

  /**
   * Looks up and returns the index of the first column in the schema with the specified name.
   * If multiple columns have the same name, the first such index is returned.
   * @param col_name name of column to look for
   * @return the index of a column with the given name, throws an exception if it does not exist
   */
  uint32_t GetColIdx(const std::string &col_name) const {
    for (uint32_t i = 0; i < columns_.size(); ++i) {
      if (columns_[i].GetName() == col_name) {
        return i;
      }
    }
    UNREACHABLE("Column does not exist");
  }

  /** @return the indices of non-inlined columns */
  const std::vector<uint32_t> &GetUnlinedColumns() const { return uninlined_columns_; }

  /** @return the number of columns in the schema for the tuple */
  uint32_t GetColumnCount() const { return static_cast<uint32_t>(columns_.size()); }

  /** @return the number of non-inlined columns */
  uint32_t GetUnlinedColumnCount() const { return static_cast<uint32_t>(uninlined_columns_.size()); }

  /** @return the number of bytes used by one tuple */
  inline uint32_t GetLength() const { return length_; }

  /** @return true if all columns are inlined, false otherwise */
  inline bool IsInlined() const { return tuple_is_inlined_; }

  /** @return string representation of this schema */
  std::string ToString() const;

 private:
  /** Fixed-length column size, i.e. the number of bytes used by one tuple. */
  uint32_t length_;

  /** All the columns in the schema, inlined and uninlined. */
  std::vector<Column> columns_;

  /** True if all the columns are inlined, false otherwise. */
  bool tuple_is_inlined_;

  /** Indices of all uninlined columns. */
  std::vector<uint32_t> uninlined_columns_;
};

}  // namespace bustub
