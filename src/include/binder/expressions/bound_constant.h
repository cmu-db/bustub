#pragma once

#include <fmt/format.h>
#include <cassert>
#include <string>
#include <utility>
#include "../bound_expression.h"
#include "type/limits.h"

namespace bustub {
class BoundConstant : public BoundExpression {
 public:
  explicit BoundConstant(duckdb_libpgquery::PGValue val) : BoundExpression(ExpressionType::CONSTANT), val_(0) {
    switch (val.type) {
      case duckdb_libpgquery::T_PGInteger:
        assert(val.val.ival <= BUSTUB_INT32_MAX);
        val_ = val.val.ival;
        break;
      default:
        throw Exception(fmt::format("unsupported pg value: {}", Parser::NodetypeToString(val.type)));
    }
  }

  auto ToString() const -> std::string override { return fmt::format("{}", val_); }

  // The value of the constant, currently only support int, will add more in the future.
  int val_;
};
}  // namespace bustub
