//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// column.h
//
// Identification: src/include/catalog/column.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "common/exception.h"
#include "common/macros.h"
#include "type/type.h"

namespace bustub {

class Column {
  friend class Schema;

 public:
  /**
   * Non-variable-length constructor for creating a Column.
   * @param column_name name of the column
   * @param type type of the column
   */
  Column(std::string column_name, TypeId type)
      : column_name_(std::move(column_name)), column_type_(type), fixed_length_(TypeSize(type)) {
    BUSTUB_ASSERT(type != TypeId::VARCHAR, "Wrong constructor for VARCHAR type.");
  }

  /**
   * Variable-length constructor for creating a Column.
   * @param column_name name of the column
   * @param type type of column
   * @param length length of the varlen
   */
  Column(std::string column_name, TypeId type, uint32_t length)
      : column_name_(std::move(column_name)), column_type_(type), fixed_length_(TypeSize(type)) {
    BUSTUB_ASSERT(type == TypeId::VARCHAR, "Wrong constructor for non-VARCHAR type.");
  }

  /** @return column name */
  std::string GetName() const { return column_name_; }

  /** @return column length */
  uint32_t GetLength() const {
    if (IsInlined()) {
      return fixed_length_;
    }
    return variable_length_;
  }

  /** @return column fixed length */
  uint32_t GetFixedLength() const { return fixed_length_; }

  /** @return column variable length */
  uint32_t GetVariableLength() const { return variable_length_; }

  /** @return column's offset in the tuple */
  uint32_t GetOffset() const { return column_offset_; }

  /** @return column type */
  TypeId GetType() const { return column_type_; }

  /** @return true if column is inlined, false otherwise */
  bool IsInlined() const { return column_type_ != TypeId::VARCHAR; }

  /** @return a string representation of this column */
  std::string ToString() const;

 private:
  /**
   * Return the size in bytes of the type.
   * @param type type whose size is to be determined
   * @return size in bytes
   */
  static uint8_t TypeSize(TypeId type) {
    switch (type) {
      case TypeId::BOOLEAN:
        return 1;
      case TypeId::TINYINT:
        return 1;
      case TypeId::SMALLINT:
        return 2;
      case TypeId::INTEGER:
        return 4;
      case TypeId::BIGINT:
      case TypeId::DECIMAL:
      case TypeId::TIMESTAMP:
        return 8;
      case TypeId::VARCHAR:
        // TODO(Amadou): Confirm this.
        return 12;
      default: {
        UNREACHABLE("Cannot get size of invalid type");
      }
    }
  }

  /** Column name. */
  std::string column_name_;

  /** Column value's type. */
  TypeId column_type_;

  /** For a non-inlined column, this is the size of a pointer. Otherwise, the size of the fixed length column. */
  uint32_t fixed_length_;

  /** For an inlined column, 0. Otherwise, the length of the variable length column. */
  uint32_t variable_length_{0};

  /** Column offset in the tuple. */
  uint32_t column_offset_{0};
};

}  // namespace bustub
