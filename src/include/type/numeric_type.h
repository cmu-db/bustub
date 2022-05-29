//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// numeric_type.h
//
// Identification: src/include/type/numeric_type.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <cmath>

#include "type/value.h"

namespace bustub {
// A numeric value is an abstract type representing a number. Numerics can be
// either integral or non-integral (decimal), but must provide arithmetic
// operations on its value.
class NumericType : public Type {
 public:
  explicit NumericType(TypeId type) : Type(type) {}
  ~NumericType() override = default;

  // Other mathematical functions
  auto Add(const Value &left, const Value &right) const -> Value override = 0;
  auto Subtract(const Value &left, const Value &right) const -> Value override = 0;
  auto Multiply(const Value &left, const Value &right) const -> Value override = 0;
  auto Divide(const Value &left, const Value &right) const -> Value override = 0;
  auto Modulo(const Value &left, const Value &right) const -> Value override = 0;
  auto Min(const Value &left, const Value &right) const -> Value override = 0;
  auto Max(const Value &left, const Value &right) const -> Value override = 0;
  auto Sqrt(const Value &val) const -> Value override = 0;
  auto OperateNull(const Value &left, const Value &right) const -> Value override = 0;
  auto IsZero(const Value &val) const -> bool override = 0;

 protected:
  static inline auto ValMod(double x, double y) -> double {
    return x - std::trunc(static_cast<double>(x) / static_cast<double>(y)) * y;
  }
};
}  // namespace bustub
