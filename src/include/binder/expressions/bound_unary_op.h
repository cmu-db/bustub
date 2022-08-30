#pragma once

#include <fmt/format.h>
#include <cassert>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "../bound_expression.h"
#include "type/limits.h"

namespace bustub {
class BoundUnaryOp : public BoundExpression {
 public:
  explicit BoundUnaryOp(string op_name, unique_ptr<BoundExpression> arg)
      : BoundExpression(ExpressionType::UNARY_OP), op_name_(std::move(op_name)), arg_(move(arg)) {}

  auto ToString() const -> std::string override { return fmt::format("({}{})", op_name_, arg_); }

  // Operator name
  std::string op_name_;

  // Argument of the op
  unique_ptr<BoundExpression> arg_;
};
}  // namespace bustub
