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
class BoundBinaryOp : public BoundExpression {
 public:
  explicit BoundBinaryOp(string op_name, unique_ptr<BoundExpression> larg, unique_ptr<BoundExpression> rarg)
      : BoundExpression(ExpressionType::BINARY_OP),
        op_name_(std::move(op_name)),
        larg_(move(larg)),
        rarg_(move(rarg)) {}

  auto ToString() const -> std::string override { return fmt::format("({}{}{})", larg_, op_name_, rarg_); }

  // Operator name
  std::string op_name_;

  // Left argument of the op
  unique_ptr<BoundExpression> larg_;

  // Right argument of the op
  unique_ptr<BoundExpression> rarg_;
};
}  // namespace bustub
