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
  Value Add(const Value &left, const Value &right) const override = 0;
  Value Subtract(const Value &left, const Value &right) const override = 0;
  Value Multiply(const Value &left, const Value &right) const override = 0;
  Value Divide(const Value &left, const Value &right) const override = 0;
  Value Modulo(const Value &left, const Value &right) const override = 0;
  Value Min(const Value &left, const Value &right) const override = 0;
  Value Max(const Value &left, const Value &right) const override = 0;
  Value Sqrt(const Value &val) const override = 0;
  Value OperateNull(const Value &left, const Value &right) const override = 0;
  bool IsZero(const Value &val) const override = 0;

 protected:
  static inline double ValMod(double x, double y) {
    return x - std::trunc(static_cast<double>(x) / static_cast<double>(y)) * y;
  }
};
}  // namespace bustub
