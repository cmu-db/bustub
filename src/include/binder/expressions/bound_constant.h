#pragma once

#include <fmt/format.h>
#include <cassert>
#include <string>
#include <utility>
#include "../bound_expression.h"
#include "type/limits.h"

namespace bustub {

/**
 * A bound constant, e.g. `1`.
 */
class BoundConstant : public BoundExpression {
 public:
  explicit BoundConstant(const Value &val) : BoundExpression(ExpressionType::CONSTANT), val_(val) {}

  auto ToString() const -> std::string override { return val_.ToString(); }

  Value val_;
};
}  // namespace bustub
