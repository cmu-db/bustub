//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_table_ref.h
//
// Identification: src/include/binder/bound_table_ref.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>

#include "fmt/format.h"

#include "common/macros.h"

namespace bustub {

/**
 * Table reference types.
 */
enum class TableReferenceType : uint8_t {
  INVALID = 0,         /**< Invalid table reference type. */
  BASE_TABLE = 1,      /**< Base table reference. */
  JOIN = 3,            /**< Output of join. */
  CROSS_PRODUCT = 4,   /**< Output of cartesian product. */
  EXPRESSION_LIST = 5, /**< Values clause. */
  SUBQUERY = 6,        /**< Subquery. */
  CTE = 7,             /**< CTE. */
  EMPTY = 8            /**< Placeholder for empty FROM. */
};

/**
 * A bound table reference.
 */
class BoundTableRef {
 public:
  explicit BoundTableRef(TableReferenceType type) : type_(type) {}
  BoundTableRef() = default;
  virtual ~BoundTableRef() = default;

  virtual auto ToString() const -> std::string {
    switch (type_) {
      case TableReferenceType::INVALID:
        return "";
      case TableReferenceType::EMPTY:
        return "<empty>";
      default:
        // For other types of table reference, `ToString` should be derived in child classes.
        UNREACHABLE("entered unreachable code");
    }
  }

  auto IsInvalid() const -> bool { return type_ == TableReferenceType::INVALID; }

  /** The type of table reference. */
  TableReferenceType type_{TableReferenceType::INVALID};
};

}  // namespace bustub

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<bustub::BoundTableRef, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const T &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<std::unique_ptr<T>, std::enable_if_t<std::is_base_of<bustub::BoundTableRef, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<T> &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x->ToString(), ctx);
  }
};

template <>
struct fmt::formatter<bustub::TableReferenceType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::TableReferenceType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::TableReferenceType::INVALID:
        name = "Invalid";
        break;
      case bustub::TableReferenceType::BASE_TABLE:
        name = "BaseTable";
        break;
      case bustub::TableReferenceType::JOIN:
        name = "Join";
        break;
      case bustub::TableReferenceType::CROSS_PRODUCT:
        name = "CrossProduct";
        break;
      case bustub::TableReferenceType::EMPTY:
        name = "Empty";
        break;
      case bustub::TableReferenceType::EXPRESSION_LIST:
        name = "ExpressionList";
        break;
      case bustub::TableReferenceType::SUBQUERY:
        name = "Subquery";
        break;
      case bustub::TableReferenceType::CTE:
        name = "CTE";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
