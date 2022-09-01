#pragma once

#include <string>
#include <utility>
#include "binder/bound_expression.h"

namespace bustub {

/**
 * The star in SELECT list, e.g. `SELECT * FROM x`.
 */
class BoundStar : public BoundExpression {
 public:
  BoundStar() : BoundExpression(ExpressionType::STAR) {}

  auto ToString() const -> std::string override { return "*"; }
};
}  // namespace bustub
