//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_binary_op.h
//
// Identification: src/include/binder/expressions/bound_binary_op.h
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
 * A bound binary operator, e.g., `a+b`.
 */
class BoundBinaryOp : public BoundExpression {
 public:
  explicit BoundBinaryOp(std::string op_name, std::unique_ptr<BoundExpression> larg,
                         std::unique_ptr<BoundExpression> rarg)
      : BoundExpression(ExpressionType::BINARY_OP),
        op_name_(std::move(op_name)),
        larg_(std::move(larg)),
        rarg_(std::move(rarg)) {}

  auto ToString() const -> std::string override { return fmt::format("({}{}{})", larg_, op_name_, rarg_); }

  auto HasAggregation() const -> bool override { return larg_->HasAggregation() || rarg_->HasAggregation(); }

  /** Operator name. */
  std::string op_name_;

  /** Left argument of the op. */
  std::unique_ptr<BoundExpression> larg_;

  /** Right argument of the op. */
  std::unique_ptr<BoundExpression> rarg_;
};
}  // namespace bustub
