//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// integer_type.cpp
//
// Identification: src/type/integer_type.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include "type/integer_type.h"

namespace bustub {
#define INT_COMPARE_FUNC(OP)                                               \
  switch (right.GetTypeId()) {                                             \
    case TypeId::TINYINT:                                                  \
      return GetCmpBool(left.value_.integer_ OP right.GetAs<int8_t>());    \
    case TypeId::SMALLINT:                                                 \
      return GetCmpBool(left.value_.integer_ OP right.GetAs<int16_t>());   \
    case TypeId::INTEGER:                                                  \
      return GetCmpBool(left.value_.integer_ OP right.GetAs<int32_t>());   \
    case TypeId::BIGINT:                                                   \
      return GetCmpBool(left.value_.integer_ OP right.GetAs<int64_t>());   \
    case TypeId::DECIMAL:                                                  \
      return GetCmpBool(left.value_.integer_ OP right.GetAs<double>());    \
    case TypeId::VARCHAR: {                                                \
      auto r_value = right.CastAs(TypeId::INTEGER);                        \
      return GetCmpBool(left.value_.integer_ OP r_value.GetAs<int32_t>()); \
    }                                                                      \
    default:                                                               \
      break;                                                               \
  }  // SWITCH

#define INT_MODIFY_FUNC(METHOD, OP)                                                 \
  switch (right.GetTypeId()) {                                                      \
    case TypeId::TINYINT:                                                           \
      /* NOLINTNEXTLINE */                                                          \
      return METHOD<int32_t, int8_t>(left, right);                                  \
    case TypeId::SMALLINT:                                                          \
      /* NOLINTNEXTLINE */                                                          \
      return METHOD<int32_t, int16_t>(left, right);                                 \
    case TypeId::INTEGER:                                                           \
      /* NOLINTNEXTLINE */                                                          \
      return METHOD<int32_t, int32_t>(left, right);                                 \
    case TypeId::BIGINT:                                                            \
      /* NOLINTNEXTLINE */                                                          \
      return METHOD<int32_t, int64_t>(left, right);                                 \
    case TypeId::DECIMAL:                                                           \
      return Value(TypeId::DECIMAL, left.value_.integer_ OP right.GetAs<double>()); \
    case TypeId::VARCHAR: {                                                         \
      auto r_value = right.CastAs(TypeId::INTEGER);                                 \
      /* NOLINTNEXTLINE */                                                          \
      return METHOD<int32_t, int32_t>(left, r_value);                               \
    }                                                                               \
    default:                                                                        \
      break;                                                                        \
  }  // SWITCH

IntegerType::IntegerType(TypeId type) : IntegerParentType(type) {}

bool IntegerType::IsZero(const Value &val) const { return (val.value_.integer_ == 0); }

Value IntegerType::Add(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  INT_MODIFY_FUNC(AddValue, +);
  throw Exception("type error");
}

Value IntegerType::Subtract(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  INT_MODIFY_FUNC(SubtractValue, -);

  throw Exception("type error");
}

Value IntegerType::Multiply(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  INT_MODIFY_FUNC(MultiplyValue, *);

  throw Exception("type error");
}

Value IntegerType::Divide(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  if (right.IsZero()) {
    throw Exception(ExceptionType::DIVIDE_BY_ZERO, "Division by zero on right-hand side");
  }

  INT_MODIFY_FUNC(DivideValue, /);

  throw Exception("type error");
}

Value IntegerType::Modulo(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  if (right.IsZero()) {
    throw Exception(ExceptionType::DIVIDE_BY_ZERO, "Division by zero on right-hand side");
  }

  switch (right.GetTypeId()) {
    case TypeId::TINYINT:
      return ModuloValue<int32_t, int8_t>(left, right);
    case TypeId::SMALLINT:
      return ModuloValue<int32_t, int16_t>(left, right);
    case TypeId::INTEGER:
      return ModuloValue<int32_t, int32_t>(left, right);
    case TypeId::BIGINT:
      return ModuloValue<int32_t, int64_t>(left, right);
    case TypeId::DECIMAL:
      return Value(TypeId::DECIMAL, ValMod(left.value_.integer_, right.GetAs<double>()));
    case TypeId::VARCHAR: {
      auto r_value = right.CastAs(TypeId::INTEGER);
      return ModuloValue<int32_t, int32_t>(left, r_value);
    }
    default:
      break;
  }

  throw Exception("type error");
}

Value IntegerType::Sqrt(const Value &val) const {
  assert(val.CheckInteger());
  if (val.IsNull()) {
    return OperateNull(val, val);
  }

  if (val.value_.integer_ < 0) {
    throw Exception(ExceptionType::DECIMAL, "Cannot take square root of a negative number.");
  }
  return Value(TypeId::DECIMAL, std::sqrt(val.value_.integer_));
}

Value IntegerType::OperateNull(const Value &left __attribute__((unused)), const Value &right) const {
  switch (right.GetTypeId()) {
    case TypeId::TINYINT:
    case TypeId::SMALLINT:
    case TypeId::INTEGER:
      return Value(TypeId::INTEGER, static_cast<int32_t>(BUSTUB_INT32_NULL));
    case TypeId::BIGINT:
      return Value(TypeId::BIGINT, static_cast<int64_t>(BUSTUB_INT64_NULL));
    case TypeId::DECIMAL:
      return Value(TypeId::DECIMAL, static_cast<double>(BUSTUB_DECIMAL_NULL));
    default:
      break;
  }

  throw Exception("type error");
}

CmpBool IntegerType::CompareEquals(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));

  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  INT_COMPARE_FUNC(==);  // NOLINT

  throw Exception("type error");
}

CmpBool IntegerType::CompareNotEquals(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  INT_COMPARE_FUNC(!=);  // NOLINT

  throw Exception("type error");
}

CmpBool IntegerType::CompareLessThan(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  INT_COMPARE_FUNC(<);  // NOLINT

  throw Exception("type error");
}

CmpBool IntegerType::CompareLessThanEquals(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  INT_COMPARE_FUNC(<=);  // NOLINT

  throw Exception("type error");
}

CmpBool IntegerType::CompareGreaterThan(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  INT_COMPARE_FUNC(>);  // NOLINT

  throw Exception("type error");
}

CmpBool IntegerType::CompareGreaterThanEquals(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  INT_COMPARE_FUNC(>=);  // NOLINT

  throw Exception("type error");
}

std::string IntegerType::ToString(const Value &val) const {
  assert(val.CheckInteger());

  if (val.IsNull()) {
    return "integer_null";
  }
  return std::to_string(val.value_.integer_);
}

void IntegerType::SerializeTo(const Value &val, char *storage) const {
  *reinterpret_cast<int32_t *>(storage) = val.value_.integer_;
}

// Deserialize a value of the given type from the given storage space.
Value IntegerType::DeserializeFrom(const char *storage) const {
  int32_t val = *reinterpret_cast<const int32_t *>(storage);
  return Value(type_id_, val);
}

Value IntegerType::Copy(const Value &val) const {
  assert(val.CheckInteger());
  return Value(val.GetTypeId(), val.value_.integer_);
}

Value IntegerType::CastAs(const Value &val, const TypeId type_id) const {
  switch (type_id) {
    case TypeId::TINYINT: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_INT8_NULL);
      }
      if (val.GetAs<int32_t>() > BUSTUB_INT8_MAX || val.GetAs<int32_t>() < BUSTUB_INT8_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return Value(type_id, static_cast<int8_t>(val.GetAs<int32_t>()));
    }
    case TypeId::SMALLINT: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_INT16_NULL);
      }
      if (val.GetAs<int32_t>() > BUSTUB_INT16_MAX || val.GetAs<int32_t>() < BUSTUB_INT16_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return Value(type_id, static_cast<int16_t>(val.GetAs<int32_t>()));
    }
    case TypeId::INTEGER: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_INT32_NULL);
      }
      return Value(type_id, static_cast<int32_t>(val.GetAs<int32_t>()));
    }
    case TypeId::BIGINT: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_INT64_NULL);
      }
      return Value(type_id, static_cast<int64_t>(val.GetAs<int32_t>()));
    }
    case TypeId::DECIMAL: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_DECIMAL_NULL);
      }
      return Value(type_id, static_cast<double>(val.GetAs<int32_t>()));
    }
    case TypeId::VARCHAR: {
      if (val.IsNull()) {
        return Value(TypeId::VARCHAR, nullptr, 0, false);
      }
      return Value(TypeId::VARCHAR, val.ToString());
    }
    default:
      break;
  }
  throw Exception("Integer is not coercable to " + Type::TypeIdToString(type_id));
}
}  // namespace bustub
