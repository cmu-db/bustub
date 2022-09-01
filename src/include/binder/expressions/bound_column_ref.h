#pragma once

#include <string>
#include <utility>
#include "binder/bound_expression.h"

namespace bustub {

/**
 * A bound column reference, e.g., `y.x` in the SELECT list.
 */
class BoundColumnRef : public BoundExpression {
 public:
  explicit BoundColumnRef(std::string table, std::string col)
      : BoundExpression(ExpressionType::COLUMN_REF), table_(std::move(table)), col_(std::move(col)) {}

  auto ToString() const -> std::string override { return fmt::format("{}.{}", table_, col_); }

  /** Name of the table. */
  std::string table_;

  /** The position of the column in the schema. */
  std::string col_;
};
}  // namespace bustub
