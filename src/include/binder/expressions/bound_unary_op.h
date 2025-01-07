//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_unary_op.h
//
// Identification: src/include/binder/expressions/bound_unary_op.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "binder/bound_expression.h"

namespace bustub {

/**
 * A bound unary operation, e.g., `-x`.
 */
class BoundUnaryOp : public BoundExpression {
 public:
  explicit BoundUnaryOp(std::string op_name, std::unique_ptr<BoundExpression> arg)
      : BoundExpression(ExpressionType::UNARY_OP), op_name_(std::move(op_name)), arg_(std::move(arg)) {}

  auto ToString() const -> std::string override { return fmt::format("({}{})", op_name_, arg_); }

  auto HasAggregation() const -> bool override { return arg_->HasAggregation(); }

  /** Operator name. */
  std::string op_name_;

  /** Argument of the op. */
  std::unique_ptr<BoundExpression> arg_;
};
}  // namespace bustub
