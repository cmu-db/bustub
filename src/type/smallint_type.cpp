//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// smallint_type.cpp
//
// Identification: src/type/smallint_type.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>

#include "type/smallint_type.h"

namespace bustub {
#define SMALLINT_COMPARE_FUNC(OP)                                           \
  switch (right.GetTypeId()) {                                              \
    case TypeId::TINYINT:                                                   \
      return GetCmpBool(left.value_.smallint_ OP right.GetAs<int8_t>());    \
    case TypeId::SMALLINT:                                                  \
      return GetCmpBool(left.value_.smallint_ OP right.GetAs<int16_t>());   \
    case TypeId::INTEGER:                                                   \
      return GetCmpBool(left.value_.smallint_ OP right.GetAs<int32_t>());   \
    case TypeId::BIGINT:                                                    \
      return GetCmpBool(left.value_.smallint_ OP right.GetAs<int64_t>());   \
    case TypeId::DECIMAL:                                                   \
      return GetCmpBool(left.value_.smallint_ OP right.GetAs<double>());    \
    case TypeId::VARCHAR: {                                                 \
      auto r_value = right.CastAs(TypeId::SMALLINT);                        \
      return GetCmpBool(left.value_.smallint_ OP r_value.GetAs<int16_t>()); \
    }                                                                       \
    default:                                                                \
      break;                                                                \
  }  // SWITCH

#define SMALLINT_MODIFY_FUNC(METHOD, OP)                                             \
  switch (right.GetTypeId()) {                                                       \
    case TypeId::TINYINT:                                                            \
      /* NOLINTNEXTLINE */                                                           \
      return METHOD<int16_t, int8_t>(left, right);                                   \
    case TypeId::SMALLINT:                                                           \
      /* NOLINTNEXTLINE */                                                           \
      return METHOD<int16_t, int16_t>(left, right);                                  \
    case TypeId::INTEGER:                                                            \
      /* NOLINTNEXTLINE */                                                           \
      return METHOD<int16_t, int32_t>(left, right);                                  \
    case TypeId::BIGINT:                                                             \
      /* NOLINTNEXTLINE */                                                           \
      return METHOD<int16_t, int64_t>(left, right);                                  \
    case TypeId::DECIMAL:                                                            \
      return Value(TypeId::DECIMAL, left.value_.smallint_ OP right.GetAs<double>()); \
    case TypeId::VARCHAR: {                                                          \
      auto r_value = right.CastAs(TypeId::SMALLINT);                                 \
      /* NOLINTNEXTLINE */                                                           \
      return METHOD<int16_t, int16_t>(left, r_value);                                \
    }                                                                                \
    default:                                                                         \
      break;                                                                         \
  }  // SWITCH

SmallintType::SmallintType() : IntegerParentType(TypeId::SMALLINT) {}

bool SmallintType::IsZero(const Value &val) const { return (val.value_.smallint_ == 0); }

Value SmallintType::Add(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  SMALLINT_MODIFY_FUNC(AddValue, +);

  throw Exception("type error");
}

Value SmallintType::Subtract(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  SMALLINT_MODIFY_FUNC(SubtractValue, -);

  throw Exception("type error");
}

Value SmallintType::Multiply(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  SMALLINT_MODIFY_FUNC(MultiplyValue, *);

  throw Exception("type error");
}

Value SmallintType::Divide(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  if (right.IsZero()) {
    throw Exception(ExceptionType::DIVIDE_BY_ZERO, "Division by zero on right-hand side");
  }

  SMALLINT_MODIFY_FUNC(DivideValue, /);

  throw Exception("type error");
}

Value SmallintType::Modulo(const Value &left, const Value &right) const {
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
      return ModuloValue<int16_t, int8_t>(left, right);
    case TypeId::SMALLINT:
      return ModuloValue<int16_t, int16_t>(left, right);
    case TypeId::INTEGER:
      return ModuloValue<int16_t, int32_t>(left, right);
    case TypeId::BIGINT:
      return ModuloValue<int16_t, int64_t>(left, right);
    case TypeId::DECIMAL:
      return Value(TypeId::DECIMAL, ValMod(left.value_.smallint_, right.GetAs<double>()));
    case TypeId::VARCHAR: {
      auto r_value = right.CastAs(TypeId::SMALLINT);
      return ModuloValue<int16_t, int16_t>(left, r_value);
    }
    default:
      break;
  }
  throw Exception("type error");
}

Value SmallintType::Sqrt(const Value &val) const {
  assert(val.CheckInteger());
  if (val.IsNull()) {
    return Value(TypeId::DECIMAL, static_cast<double>(BUSTUB_DECIMAL_NULL));
  }

  if (val.value_.smallint_ < 0) {
    throw Exception(ExceptionType::DECIMAL, "Cannot take square root of a negative number.");
  }
  return Value(TypeId::DECIMAL, std::sqrt(val.value_.smallint_));
}

Value SmallintType::OperateNull(const Value &left __attribute__((unused)), const Value &right) const {
  switch (right.GetTypeId()) {
    case TypeId::TINYINT:
    case TypeId::SMALLINT:
      return Value(TypeId::SMALLINT, BUSTUB_INT16_NULL);
    case TypeId::INTEGER:
      return Value(TypeId::INTEGER, BUSTUB_INT32_NULL);
    case TypeId::BIGINT:
      return Value(TypeId::BIGINT, BUSTUB_INT64_NULL);
    case TypeId::DECIMAL:
      return Value(TypeId::DECIMAL, static_cast<double>(BUSTUB_DECIMAL_NULL));
    default:
      break;
  }

  throw Exception("type error");
}

CmpBool SmallintType::CompareEquals(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));

  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  SMALLINT_COMPARE_FUNC(==);  // NOLINT

  throw Exception("type error");
}

CmpBool SmallintType::CompareNotEquals(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  SMALLINT_COMPARE_FUNC(!=);  // NOLINT

  throw Exception("type error");
}

CmpBool SmallintType::CompareLessThan(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  SMALLINT_COMPARE_FUNC(<);  // NOLINT

  throw Exception("type error");
}

CmpBool SmallintType::CompareLessThanEquals(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  SMALLINT_COMPARE_FUNC(<=);  // NOLINT

  throw Exception("type error");
}

CmpBool SmallintType::CompareGreaterThan(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  SMALLINT_COMPARE_FUNC(>);  // NOLINT

  throw Exception("type error");
}

CmpBool SmallintType::CompareGreaterThanEquals(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }

  SMALLINT_COMPARE_FUNC(>=);  // NOLINT

  throw Exception("type error");
}

std::string SmallintType::ToString(const Value &val) const {
  assert(val.CheckInteger());
  switch (val.GetTypeId()) {
    case TypeId::TINYINT:
      if (val.IsNull()) {
        return "tinyint_null";
      }
      return std::to_string(val.value_.tinyint_);
    case TypeId::SMALLINT:
      if (val.IsNull()) {
        return "smallint_null";
      }
      return std::to_string(val.value_.smallint_);
    case TypeId::INTEGER:
      if (val.IsNull()) {
        return "integer_null";
      }
      return std::to_string(val.value_.integer_);
    case TypeId::BIGINT:
      if (val.IsNull()) {
        return "bigint_null";
      }
      return std::to_string(val.value_.bigint_);
    default:
      break;
  }
  throw Exception("type error");
}

void SmallintType::SerializeTo(const Value &val, char *storage) const {
  *reinterpret_cast<int16_t *>(storage) = val.value_.smallint_;
}

// Deserialize a value of the given type from the given storage space.
Value SmallintType::DeserializeFrom(const char *storage) const {
  int16_t val = *reinterpret_cast<const int16_t *>(storage);
  return Value(type_id_, val);
}

Value SmallintType::Copy(const Value &val) const {
  assert(val.CheckInteger());

  return Value(TypeId::SMALLINT, val.value_.smallint_);

  throw Exception("type error");
}

Value SmallintType::CastAs(const Value &val, const TypeId type_id) const {
  switch (type_id) {
    case TypeId::TINYINT: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_INT8_NULL);
      }
      if (val.GetAs<int16_t>() > BUSTUB_INT8_MAX || val.GetAs<int16_t>() < BUSTUB_INT8_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return Value(type_id, static_cast<int8_t>(val.GetAs<int16_t>()));
    }
    case TypeId::SMALLINT: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_INT16_NULL);
      }
      return Copy(val);
    }
    case TypeId::INTEGER: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_INT32_NULL);
      }
      return Value(type_id, static_cast<int32_t>(val.GetAs<int16_t>()));
    }
    case TypeId::BIGINT: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_INT64_NULL);
      }
      return Value(type_id, static_cast<int64_t>(val.GetAs<int16_t>()));
    }
    case TypeId::DECIMAL: {
      if (val.IsNull()) {
        return Value(type_id, BUSTUB_DECIMAL_NULL);
      }
      return Value(type_id, static_cast<double>(val.GetAs<int16_t>()));
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
  throw Exception("smallint is not coercable to " + Type::TypeIdToString(type_id));
}
}  // namespace bustub
