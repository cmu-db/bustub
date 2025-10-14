//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_order_by.h
//
// Identification: src/include/binder/bound_order_by.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include "binder/bound_expression.h"
#include "common/exception.h"
#include "execution/expressions/abstract_expression.h"
#include "fmt/format.h"

namespace bustub {

/**
 * All types of order-bys in binder.
 */
enum class OrderByType : uint8_t {
  INVALID = 0, /**< Invalid order by type. */
  DEFAULT = 1, /**< Default order by type. */
  ASC = 2,     /**< Ascending order by type. */
  DESC = 3,    /**< Descending order by type. */
};

/**
 * All types of order by nulls in binder.
 */
enum class OrderByNullType : uint8_t {
  DEFAULT = 0,     /**< Default order by type. */
  NULLS_FIRST = 1, /**< Ascending order by type. */
  NULLS_LAST = 2,  /**< Descending order by type. */
};

using OrderBy = std::tuple<OrderByType, OrderByNullType, AbstractExpressionRef>;

/**
 * BoundOrderBy is an item in the ORDER BY clause.
 */
class BoundOrderBy {
 public:
  BoundOrderBy(OrderByType type, OrderByNullType null_order, std::unique_ptr<BoundExpression> expr)
      : type_(type), null_order_(null_order), expr_(std::move(expr)) {}

  /** The order by type (ASC/DESC/DEFAULT). */
  OrderByType type_;

  /** The null ordering (NULLS FIRST/LAST/DEFAULT). */
  OrderByNullType null_order_;

  /** The order by expression */
  std::unique_ptr<BoundExpression> expr_;

  /** Render this statement as a string. */
  auto ToString() const -> std::string {
    return fmt::format("BoundOrderBy {{ type={}, nulls={}, expr={} }}", type_, null_order_, expr_);
  }
};

}  // namespace bustub

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of<bustub::BoundOrderBy, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const bustub::BoundOrderBy &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x.ToString(), ctx);
  }
};

template <typename T>
struct fmt::formatter<std::unique_ptr<T>, std::enable_if_t<std::is_base_of<bustub::BoundOrderBy, T>::value, char>>
    : fmt::formatter<std::string> {
  template <typename FormatCtx>
  auto format(const std::unique_ptr<bustub::BoundOrderBy> &x, FormatCtx &ctx) const {
    return fmt::formatter<std::string>::format(x->ToString(), ctx);
  }
};

template <>
struct fmt::formatter<bustub::OrderByType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::OrderByType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::OrderByType::INVALID:
        name = "Invalid";
        break;
      case bustub::OrderByType::ASC:
        name = "Ascending";
        break;
      case bustub::OrderByType::DESC:
        name = "Descending";
        break;
      case bustub::OrderByType::DEFAULT:
        name = "Default";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

template <>
struct fmt::formatter<bustub::OrderByNullType> : fmt::formatter<std::string_view> {
  template <typename FormatContext>
  auto format(bustub::OrderByNullType c, FormatContext &ctx) const {
    std::string_view name;
    switch (c) {
      case bustub::OrderByNullType::DEFAULT:
        name = "Default";
        break;
      case bustub::OrderByNullType::NULLS_FIRST:
        name = "NullsFirst";
        break;
      case bustub::OrderByNullType::NULLS_LAST:
        name = "NullsLast";
        break;
      default:
        name = "Unknown";
        break;
    }
    return fmt::formatter<std::string_view>::format(name, ctx);
  }
};
