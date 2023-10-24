#pragma once

#include <memory>
#include <string>
#include <utility>
#include "binder/bound_expression.h"

namespace bustub {

/**
 * The alias in SELECT list, e.g. `SELECT count(x) AS y`, the `y` is an alias.
 */
class BoundAlias : public BoundExpression {
 public:
  explicit BoundAlias(std::string alias, std::unique_ptr<BoundExpression> child)
      : BoundExpression(ExpressionType::ALIAS), alias_(std::move(alias)), child_(std::move(child)) {}

  auto ToString() const -> std::string override { return fmt::format("({} as {})", child_, alias_); }

  auto HasAggregation() const -> bool override { return child_->HasAggregation(); }

  auto HasWindowFunction() const -> bool override { return child_->HasWindowFunction(); }

  /** Alias name. */
  std::string alias_;

  /** The actual expression */
  std::unique_ptr<BoundExpression> child_;
};
}  // namespace bustub
