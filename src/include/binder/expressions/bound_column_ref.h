#pragma once

#include <string>
#include <utility>
#include <vector>
#include "binder/bound_expression.h"

namespace bustub {

/**
 * A bound column reference, e.g., `y.x` in the SELECT list.
 */
class BoundColumnRef : public BoundExpression {
 public:
  explicit BoundColumnRef(std::vector<std::string> col_name)
      : BoundExpression(ExpressionType::COLUMN_REF), col_name_(std::move(col_name)) {}

  auto ToString() const -> std::string override { return fmt::format("{}", fmt::join(col_name_, ".")); }

  auto HasAggregation() const -> bool override { return false; }

  /** The name of the column. */
  std::vector<std::string> col_name_;
};
}  // namespace bustub
