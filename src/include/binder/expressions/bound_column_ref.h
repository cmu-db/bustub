//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_column_ref.h
//
// Identification: src/include/binder/expressions/bound_column_ref.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "binder/bound_expression.h"
#include "common/macros.h"
#include "fmt/ranges.h"

namespace bustub {

/**
 * A bound column reference, e.g., `y.x` in the SELECT list.
 */
class BoundColumnRef : public BoundExpression {
 public:
  explicit BoundColumnRef(std::vector<std::string> col_name)
      : BoundExpression(ExpressionType::COLUMN_REF), col_name_(std::move(col_name)) {}

  static auto Prepend(std::unique_ptr<BoundColumnRef> self, std::string prefix) -> std::unique_ptr<BoundColumnRef> {
    if (self == nullptr) {
      return nullptr;
    }
    std::vector<std::string> col_name{std::move(prefix)};
    std::copy(self->col_name_.cbegin(), self->col_name_.cend(), std::back_inserter(col_name));
    return std::make_unique<BoundColumnRef>(std::move(col_name));
  }

  auto ToString() const -> std::string override { return fmt::format("{}", fmt::join(col_name_, ".")); }

  auto HasAggregation() const -> bool override { return false; }

  /** The name of the column. */
  std::vector<std::string> col_name_;
};
}  // namespace bustub
