#pragma once

#include <fmt/format.h>
#include <cassert>
#include <string>
#include <utility>
#include "../bound_expression.h"
#include "type/limits.h"

namespace bustub {

/**
 * A bound column ref. e.g. `y.x` in the select list.
 */
class BoundColumnRef : public BoundExpression {
 public:
  explicit BoundColumnRef(string table, string col)
      : BoundExpression(ExpressionType::COLUMN_REF), table_(std::move(table)), col_(std::move(col)) {}

  auto ToString() const -> std::string override { return fmt::format("{}.{}", table_, col_); }

  // Name of the table
  string table_;

  // The position of the column in the schema
  string col_;
};
}  // namespace bustub
