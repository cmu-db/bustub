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
#include <memory>
#include <string>
#include <utility>

#include "fmt/format.h"

#include "common/exception.h"
#include "common/macros.h"
#include "type/type.h"

namespace bustub {
class AbstractExpression;

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
   * @param expr expression used to create this column
   */
  Column(std::string column_name, TypeId type, uint32_t length)
      : column_name_(std::move(column_name)),
        column_type_(type),
        fixed_length_(TypeSize(type)),
        variable_length_(length) {
    BUSTUB_ASSERT(type == TypeId::VARCHAR, "Wrong constructor for non-VARCHAR type.");
  }

  /**
   * Replicate a Column with a different name.
   * @param column_name name of the column
   * @param column the original column
   */
  Column(std::string column_name, const Column &column)
      : column_name_(std::move(column_name)),
        column_type_(column.column_type_),
        fixed_length_(column.fixed_length_),
        variable_length_(column.variable_length_),
        column_offset_(column.column_offset_) {}

  /** @return column name */
  auto GetName() const -> std::string { return column_name_; }

  /** @return column length */
  auto GetLength() const -> uint32_t {
    if (IsInlined()) {
      return fixed_length_;
    }
    return variable_length_;
  }

  /** @return column fixed length */
  auto GetFixedLength() const -> uint32_t { return fixed_length_; }

  /** @return column variable length */
  auto GetVariableLength() const -> uint32_t { return variable_length_; }

  /** @return column's offset in the tuple */
  auto GetOffset() const -> uint32_t { return column_offset_; }

  /** @return column type */
  auto GetType() const -> TypeId { return column_type_; }

  /** @return true if column is inlined, false otherwise */
  auto IsInlined() const -> bool { return column_type_ != TypeId::VARCHAR; }

  /** @return a string representation of this column */
  auto ToString(bool simplified = true) const -> std::string;

 private:
  /**
   * Return the size in bytes of the type.
   * @param type type whose size is to be determined
   * @return size in bytes
   */
  static auto TypeSize(TypeId type) -> uint8_t {
    switch (type) {
      case TypeId::BOOLEAN:
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

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<bustub::Column, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const bustub::Column &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<std::unique_ptr<T>, std::enable_if_t<std::is_base_of<bustub::Column, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<bustub::Column> &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x->ToString(), ctx);
  }
};
