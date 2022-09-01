#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "binder/bound_expression.h"

namespace bustub {

/**
 * A bound aggregate call, e.g., `sum(x)`.
 */
class BoundAggCall : public BoundExpression {
 public:
  explicit BoundAggCall(std::string func_name, std::vector<std::unique_ptr<BoundExpression>> args)
      : BoundExpression(ExpressionType::AGG_CALL), func_name_(std::move(func_name)), args_(move(args)) {}

  auto ToString() const -> std::string override;

  /** Function name. */
  std::string func_name_;

  /** Arguments of the agg call. */
  std::vector<std::unique_ptr<BoundExpression>> args_;
};
}  // namespace bustub
