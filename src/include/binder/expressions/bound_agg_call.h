#pragma once

#include <fmt/format.h>
#include <cassert>
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
  explicit BoundAggCall(string func_name, vector<unique_ptr<BoundExpression>> args)
      : BoundExpression(ExpressionType::AGG_CALL), func_name_(std::move(func_name)), args_(move(args)) {}

  auto ToString() const -> std::string override { return fmt::format("{}({})", func_name_, fmt::join(args_, ", ")); }

  /** Function name. */
  std::string func_name_;

  /** Arguments of the agg call. */
  vector<unique_ptr<BoundExpression>> args_;
};
}  // namespace bustub
