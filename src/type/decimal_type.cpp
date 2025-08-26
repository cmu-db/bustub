//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// decimal_type.cpp
//
// Identification: src/type/decimal_type.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include "common/exception.h"
#include "type/decimal_type.h"

namespace bustub {
#define DECIMAL_COMPARE_FUNC(OP)                                          \
  switch (right.GetTypeId()) {                                            \
    case TypeId::TINYINT:                                                 \
      return GetCmpBool(left.value_.decimal_ OP right.GetAs<int8_t>());   \
    case TypeId::SMALLINT:                                                \
      return GetCmpBool(left.value_.decimal_ OP right.GetAs<int16_t>());  \
    case TypeId::INTEGER:                                                 \
      return GetCmpBool(left.value_.decimal_ OP right.GetAs<int32_t>());  \
    case TypeId::BIGINT:                                                  \
      return GetCmpBool(left.value_.decimal_ OP right.GetAs<int64_t>());  \
    case TypeId::DECIMAL:                                                 \
      return GetCmpBool(left.value_.decimal_ OP right.GetAs<double>());   \
    case TypeId::VARCHAR: {                                               \
      auto r_value = right.CastAs(TypeId::DECIMAL);                       \
      return GetCmpBool(left.value_.decimal_ OP r_value.GetAs<double>()); \
    }                                                                     \
    default:                                                              \
      break;                                                              \
  }  // SWITCH

#define DECIMAL_MODIFY_FUNC(OP)                                                       \
  switch (right.GetTypeId()) {                                                        \
    case TypeId::TINYINT:                                                             \
      return Value(TypeId::DECIMAL, left.value_.decimal_ OP right.GetAs<int8_t>());   \
    case TypeId::SMALLINT:                                                            \
      return Value(TypeId::DECIMAL, left.value_.decimal_ OP right.GetAs<int16_t>());  \
    case TypeId::INTEGER:                                                             \
      return Value(TypeId::DECIMAL, left.value_.decimal_ OP right.GetAs<int32_t>());  \
    case TypeId::BIGINT:                                                              \
      return Value(TypeId::DECIMAL, left.value_.decimal_ OP right.GetAs<int64_t>());  \
    case TypeId::DECIMAL:                                                             \
      return Value(TypeId::DECIMAL, left.value_.decimal_ OP right.GetAs<double>());   \
    case TypeId::VARCHAR: {                                                           \
      auto r_value = right.CastAs(TypeId::DECIMAL);                                   \
      return Value(TypeId::DECIMAL, left.value_.decimal_ OP r_value.GetAs<double>()); \
    }                                                                                 \
    default:                                                                          \
      break;                                                                          \
  }  // SWITCH

// static inline double ValMod(double x, double y) {
//  return x - std::trunc((double)x / (double)y) * y;
//}

DecimalType::DecimalType() : NumericType(TypeId::DECIMAL) {}

auto DecimalType::IsZero(const Value &val) const -> bool {
  assert(GetTypeId() == TypeId::DECIMAL);
  return (val.value_.decimal_ == 0);
}

auto DecimalType::Add(const Value &left, const Value &right) const -> Value {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  DECIMAL_MODIFY_FUNC(+);  // NOLINT
  throw Exception("type error");
}

auto DecimalType::Subtract(const Value &left, const Value &right) const -> Value {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  DECIMAL_MODIFY_FUNC(-);  // NOLINT

  throw Exception("type error");
}

auto DecimalType::Multiply(const Value &left, const Value &right) const -> Value {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  DECIMAL_MODIFY_FUNC(*);  // NOLINT
  throw Exception("type error");
}

auto DecimalType::Divide(const Value &left, const Value &right) const -> Value {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  if (right.IsZero()) {
    throw Exception(ExceptionType::DIVIDE_BY_ZERO, "Division by zero on right-hand side");
  }

  DECIMAL_MODIFY_FUNC(/);  // NOLINT

  throw Exception("type error");
}

auto DecimalType::Modulo(const Value &left, const Value &right) const -> Value {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return OperateNull(left, right);
  }

  if (right.IsZero()) {
    throw Exception(ExceptionType::DIVIDE_BY_ZERO, "Division by zero on right-hand side");
  }
  switch (right.GetTypeId()) {
    case TypeId::TINYINT:
      return {TypeId::DECIMAL, ValMod(left.value_.decimal_, right.GetAs<int8_t>())};
    case TypeId::SMALLINT:
      return {TypeId::DECIMAL, ValMod(left.value_.decimal_, right.GetAs<int16_t>())};
    case TypeId::INTEGER:
      return {TypeId::DECIMAL, ValMod(left.value_.decimal_, right.GetAs<int32_t>())};
    case TypeId::BIGINT:
      return {TypeId::DECIMAL, ValMod(left.value_.decimal_, right.GetAs<int64_t>())};
    case TypeId::DECIMAL:
      return {TypeId::DECIMAL, ValMod(left.value_.decimal_, right.GetAs<double>())};
    case TypeId::VARCHAR: {
      auto r_value = right.CastAs(TypeId::DECIMAL);
      return {TypeId::DECIMAL, ValMod(left.value_.decimal_, r_value.GetAs<double>())};
    }
    default:
      break;
  }
  throw Exception("type error");
}

auto DecimalType::Min(const Value &left, const Value &right) const -> Value {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  if (left.CompareLessThanEquals(right) == CmpBool::CmpTrue) {
    return left.Copy();
  }
  return right.Copy();
}

auto DecimalType::Max(const Value &left, const Value &right) const -> Value {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  if (left.CompareGreaterThanEquals(right) == CmpBool::CmpTrue) {
    return left.Copy();
  }
  return right.Copy();
}

auto DecimalType::Sqrt(const Value &val) const -> Value {
  assert(GetTypeId() == TypeId::DECIMAL);
  if (val.IsNull()) {
    return {TypeId::DECIMAL, BUSTUB_DECIMAL_NULL};
  }
  if (val.value_.decimal_ < 0) {
    throw Exception(ExceptionType::DECIMAL, "Cannot take square root of a negative number.");
  }
  return {TypeId::DECIMAL, std::sqrt(val.value_.decimal_)};
}

auto DecimalType::OperateNull(const Value &left __attribute__((unused)),
                              const Value &right __attribute__((unused))) const -> Value {
  return {TypeId::DECIMAL, BUSTUB_DECIMAL_NULL};
}

auto DecimalType::CompareEquals(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  DECIMAL_COMPARE_FUNC(==);  // NOLINT

  throw Exception("type error");
}

auto DecimalType::CompareNotEquals(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  DECIMAL_COMPARE_FUNC(!=);  // NOLINT

  throw Exception("type error");
}

auto DecimalType::CompareLessThan(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  DECIMAL_COMPARE_FUNC(<);  // NOLINT

  throw Exception("type error");
}

auto DecimalType::CompareLessThanEquals(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  DECIMAL_COMPARE_FUNC(<=);  // NOLINT

  throw Exception("type error");
}

auto DecimalType::CompareGreaterThan(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  DECIMAL_COMPARE_FUNC(>);  // NOLINT

  throw Exception("type error");
}

auto DecimalType::CompareGreaterThanEquals(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::DECIMAL);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  DECIMAL_COMPARE_FUNC(>=);  // NOLINT

  throw Exception("type error");
}

auto DecimalType::CastAs(const Value &val, const TypeId type_id) const -> Value {
  switch (type_id) {
    case TypeId::TINYINT: {
      if (val.IsNull()) {
        return {type_id, BUSTUB_INT8_NULL};
      }
      if (val.GetAs<double>() > BUSTUB_INT8_MAX || val.GetAs<double>() < BUSTUB_INT8_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return {type_id, static_cast<int8_t>(val.GetAs<double>())};
    }
    case TypeId::SMALLINT: {
      if (val.IsNull()) {
        return {type_id, BUSTUB_INT16_NULL};
      }
      if (val.GetAs<double>() > BUSTUB_INT16_MAX || val.GetAs<double>() < BUSTUB_INT16_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return {type_id, static_cast<int16_t>(val.GetAs<double>())};
    }
    case TypeId::INTEGER: {
      if (val.IsNull()) {
        return {type_id, BUSTUB_INT32_NULL};
      }
      if (val.GetAs<double>() > BUSTUB_INT32_MAX || val.GetAs<double>() < BUSTUB_INT32_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return {type_id, static_cast<int32_t>(val.GetAs<double>())};
    }
    case TypeId::BIGINT: {
      if (val.IsNull()) {
        return {type_id, BUSTUB_INT64_NULL};
      }
      if (val.GetAs<double>() >= static_cast<double>(BUSTUB_INT64_MAX) ||
          val.GetAs<double>() < static_cast<double>(BUSTUB_INT64_MIN)) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return {type_id, static_cast<int64_t>(val.GetAs<double>())};
    }
    case TypeId::DECIMAL:
      return val.Copy();
    case TypeId::VARCHAR: {
      if (val.IsNull()) {
        return {TypeId::VARCHAR, nullptr, 0, false};
      }
      return {TypeId::VARCHAR, val.ToString()};
    }
    default:
      break;
  }
  throw Exception("DECIMAL is not coercable to " + Type::TypeIdToString(type_id));
}

/**
 * Debug
 */
auto DecimalType::ToString(const Value &val) const -> std::string {
  if (val.IsNull()) {
    return "decimal_null";
  }
  return std::to_string(val.value_.decimal_);
}

/**
 * Serialize this value into the given storage space
 */
void DecimalType::SerializeTo(const Value &val, char *storage) const {
  *reinterpret_cast<double *>(storage) = val.value_.decimal_;
}

/**
 * Deserialize a value of the given type from the given storage space.
 */
auto DecimalType::DeserializeFrom(const char *storage) const -> Value {
  double val = *reinterpret_cast<const double *>(storage);
  return {type_id_, val};
}

/**
 * Create a copy of this value
 */
auto DecimalType::Copy(const Value &val) const -> Value { return {TypeId::DECIMAL, val.value_.decimal_}; }
}  // namespace bustub
