//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_window.h
//
// Identification: src/include/binder/expressions/bound_window.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "nodes/nodes.hpp"
#include "nodes/parsenodes.hpp"
#include "nodes/primnodes.hpp"
#include "pg_definitions.hpp"
#include "postgres_parser.hpp"

namespace bustub {

enum class WindowBoundary : uint8_t {
  INVALID = 0,
  UNBOUNDED_PRECEDING = 1,
  UNBOUNDED_FOLLOWING = 2,
  CURRENT_ROW_RANGE = 3,
  CURRENT_ROW_ROWS = 4,
  EXPR_PRECEDING_ROWS = 5,
  EXPR_FOLLOWING_ROWS = 6,
  EXPR_PRECEDING_RANGE = 7,
  EXPR_FOLLOWING_RANGE = 8
};

/**
 * A bound aggregate call, e.g., `sum(x)`.
 */
#pragma once

class BoundWindow : public BoundExpression {
 public:
  explicit BoundWindow(std::string func_name, std::vector<std::unique_ptr<BoundExpression>> args,
                       std::vector<std::unique_ptr<BoundExpression>> partition_by,
                       std::vector<std::unique_ptr<BoundOrderBy>> order_bys,
                       std::optional<std::unique_ptr<BoundExpression>> start_offset,
                       std::optional<std::unique_ptr<BoundExpression>> end_offset)
      : BoundExpression(ExpressionType::WINDOW),
        func_name_(std::move(func_name)),
        args_(std::move(args)),
        partition_by_(std::move(partition_by)),
        order_bys_(std::move(order_bys)),
        start_offset_(std::move(start_offset)),
        end_offset_(std::move(end_offset)) {}

  explicit BoundWindow(std::string func_name, std::vector<std::unique_ptr<BoundExpression>> args,
                       std::vector<std::unique_ptr<BoundExpression>> partition_by,
                       std::vector<std::unique_ptr<BoundOrderBy>> order_bys,
                       std::optional<std::unique_ptr<BoundExpression>> start_offset,
                       std::optional<std::unique_ptr<BoundExpression>> end_offset, WindowBoundary start,
                       WindowBoundary end)
      : BoundExpression(ExpressionType::WINDOW),
        func_name_(std::move(func_name)),
        args_(std::move(args)),
        partition_by_(std::move(partition_by)),
        order_bys_(std::move(order_bys)),
        start_offset_(std::move(start_offset)),
        end_offset_(std::move(end_offset)),
        start_(start),
        end_(end) {}

  auto ToString() const -> std::string override;

  auto HasAggregation() const -> bool override { return false; }

  auto HasWindowFunction() const -> bool override { return true; }

  auto SetStart(WindowBoundary start) { start_ = start; }

  auto SetEnd(WindowBoundary end) { end_ = end; }

  /** Function name. */
  std::string func_name_;

  /** Arguments of the func call. */
  std::vector<std::unique_ptr<BoundExpression>> args_;

  std::vector<std::unique_ptr<BoundExpression>> partition_by_;
  std::vector<std::unique_ptr<BoundOrderBy>> order_bys_;
  std::optional<std::unique_ptr<BoundExpression>> start_offset_;
  std::optional<std::unique_ptr<BoundExpression>> end_offset_;
  WindowBoundary start_ = WindowBoundary::INVALID;
  WindowBoundary end_ = WindowBoundary::INVALID;
};
}  // namespace bustub
