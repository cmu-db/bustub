//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// integer_parent_type.h
//
// Identification: src/include/type/integer_parent_type.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <string>
#include "common/exception.h"
#include "type/numeric_type.h"

namespace bustub {
// An integer value of the common sizes.
class IntegerParentType : public NumericType {
 public:
  ~IntegerParentType() override = default;

  explicit IntegerParentType(TypeId type);

  // Other mathematical functions
  Value Add(const Value &left, const Value &right) const override = 0;
  Value Subtract(const Value &left, const Value &right) const override = 0;
  Value Multiply(const Value &left, const Value &right) const override = 0;
  Value Divide(const Value &left, const Value &right) const override = 0;
  Value Modulo(const Value &left, const Value &right) const override = 0;
  Value Min(const Value &left, const Value &right) const override;
  Value Max(const Value &left, const Value &right) const override;
  Value Sqrt(const Value &val) const override = 0;

  // Comparison functions
  CmpBool CompareEquals(const Value &left, const Value &right) const override = 0;
  CmpBool CompareNotEquals(const Value &left, const Value &right) const override = 0;
  CmpBool CompareLessThan(const Value &left, const Value &right) const override = 0;
  CmpBool CompareLessThanEquals(const Value &left, const Value &right) const override = 0;
  CmpBool CompareGreaterThan(const Value &left, const Value &right) const override = 0;
  CmpBool CompareGreaterThanEquals(const Value &left, const Value &right) const override = 0;

  Value CastAs(const Value &val, TypeId type_id) const override = 0;

  // Integer types are always inlined
  bool IsInlined(const Value &val) const override { return true; }

  // Debug
  std::string ToString(const Value &val) const override = 0;

  // Serialize this value into the given storage space
  void SerializeTo(const Value &val, char *storage) const override = 0;

  // Deserialize a value of the given type from the given storage space.
  Value DeserializeFrom(const char *storage) const override = 0;

  // Create a copy of this value
  Value Copy(const Value &val) const override = 0;

 protected:
  template <class T1, class T2>
  Value AddValue(const Value &left, const Value &right) const;
  template <class T1, class T2>
  Value SubtractValue(const Value &left, const Value &right) const;
  template <class T1, class T2>
  Value MultiplyValue(const Value &left, const Value &right) const;
  template <class T1, class T2>
  Value DivideValue(const Value &left, const Value &right) const;
  template <class T1, class T2>
  Value ModuloValue(const Value &left, const Value &right) const;

  Value OperateNull(const Value &left, const Value &right) const override = 0;

  bool IsZero(const Value &val) const override = 0;
};

template <class T1, class T2>
Value IntegerParentType::AddValue(const Value &left, const Value &right) const {
  auto x = left.GetAs<T1>();
  auto y = right.GetAs<T2>();
  auto sum1 = static_cast<T1>(x + y);
  auto sum2 = static_cast<T2>(x + y);

  if ((x + y) != sum1 && (x + y) != sum2) {
    throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
  }
  // Overflow detection
  if (sizeof(x) >= sizeof(y)) {
    if ((x > 0 && y > 0 && sum1 < 0) || (x < 0 && y < 0 && sum1 > 0)) {
      throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
    }
    return Value(left.GetTypeId(), sum1);
  }
  if ((x > 0 && y > 0 && sum2 < 0) || (x < 0 && y < 0 && sum2 > 0)) {
    throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
  }
  return Value(right.GetTypeId(), sum2);
}

template <class T1, class T2>
Value IntegerParentType::SubtractValue(const Value &left, const Value &right) const {
  auto x = left.GetAs<T1>();
  auto y = right.GetAs<T2>();
  auto diff1 = static_cast<T1>(x - y);
  auto diff2 = static_cast<T2>(x - y);
  if ((x - y) != diff1 && (x - y) != diff2) {
    throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
  }
  // Overflow detection
  if (sizeof(x) >= sizeof(y)) {
    if ((x > 0 && y < 0 && diff1 < 0) || (x < 0 && y > 0 && diff1 > 0)) {
      throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
    }
    return Value(left.GetTypeId(), diff1);
  }
  if ((x > 0 && y < 0 && diff2 < 0) || (x < 0 && y > 0 && diff2 > 0)) {
    throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
  }
  return Value(right.GetTypeId(), diff2);
}

template <class T1, class T2>
Value IntegerParentType::MultiplyValue(const Value &left, const Value &right) const {
  auto x = left.GetAs<T1>();
  auto y = right.GetAs<T2>();
  auto prod1 = static_cast<T1>(x * y);
  auto prod2 = static_cast<T2>(x * y);
  if ((x * y) != prod1 && (x * y) != prod2) {
    throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
  }
  // Overflow detection
  if (sizeof(x) >= sizeof(y)) {
    if ((y != 0 && prod1 / y != x)) {
      throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
    }
    return Value(left.GetTypeId(), prod1);
  }
  if (y != 0 && prod2 / y != x) {
    throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
  }
  return Value(right.GetTypeId(), prod2);
}

template <class T1, class T2>
Value IntegerParentType::DivideValue(const Value &left, const Value &right) const {
  auto x = left.GetAs<T1>();
  auto y = right.GetAs<T2>();
  if (y == 0) {
    throw Exception(ExceptionType::DIVIDE_BY_ZERO, "Division by zero.");
  }
  auto quot1 = static_cast<T1>(x / y);
  auto quot2 = static_cast<T2>(x / y);
  if (sizeof(x) >= sizeof(y)) {
    return Value(left.GetTypeId(), quot1);
  }
  return Value(right.GetTypeId(), quot2);
}

template <class T1, class T2>
Value IntegerParentType::ModuloValue(const Value &left, const Value &right) const {
  auto x = left.GetAs<T1>();
  auto y = right.GetAs<T2>();
  if (y == 0) {
    throw Exception(ExceptionType::DIVIDE_BY_ZERO, "Division by zero.");
  }
  auto quot1 = static_cast<T1>(x % y);
  auto quot2 = static_cast<T2>(x % y);
  if (sizeof(x) >= sizeof(y)) {
    return Value(left.GetTypeId(), quot1);
  }
  return Value(right.GetTypeId(), quot2);
}

}  // namespace bustub
