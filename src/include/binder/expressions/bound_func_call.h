#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "binder/bound_expression.h"

namespace bustub {

/**
 * A bound func call, e.g., `lower(x)`.
 */
class BoundFuncCall : public BoundExpression {
 public:
  explicit BoundFuncCall(std::string func_name, std::vector<std::unique_ptr<BoundExpression>> args)
      : BoundExpression(ExpressionType::FUNC_CALL), func_name_(std::move(func_name)), args_(std::move(args)) {}

  auto ToString() const -> std::string override;

  auto HasAggregation() const -> bool override { return false; }

  /** Function name. */
  std::string func_name_;

  /** Arguments of the func call. */
  std::vector<std::unique_ptr<BoundExpression>> args_;
};
}  // namespace bustub
