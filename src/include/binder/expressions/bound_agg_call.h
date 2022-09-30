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
  explicit BoundAggCall(std::string func_name, bool is_distinct, std::vector<std::unique_ptr<BoundExpression>> args)
      : BoundExpression(ExpressionType::AGG_CALL),
        func_name_(std::move(func_name)),
        is_distinct_(is_distinct),
        args_(std::move(args)) {}

  auto ToString() const -> std::string override;

  auto HasAggregation() const -> bool override { return true; }

  /** Function name. */
  std::string func_name_;

  /** Is distinct aggregation */
  bool is_distinct_;

  /** Arguments of the agg call. */
  std::vector<std::unique_ptr<BoundExpression>> args_;
};
}  // namespace bustub
