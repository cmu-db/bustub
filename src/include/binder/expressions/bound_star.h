#pragma once

#include <fmt/format.h>
#include <cassert>
#include <string>
#include <utility>
#include "../bound_expression.h"
#include "type/limits.h"

namespace bustub {
class BoundStar : public BoundExpression {
 public:
  BoundStar() : BoundExpression(ExpressionType::STAR) {}

  auto ToString() const -> std::string override { return "*"; }
};
}  // namespace bustub
