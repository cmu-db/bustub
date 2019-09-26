//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// boolean_type.cpp
//
// Identification: src/type/boolean_type.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>

#include <string>
#include "type/boolean_type.h"

namespace bustub {
#define BOOLEAN_COMPARE_FUNC(OP) GetCmpBool(left.value_.boolean_ OP right.CastAs(TypeId::BOOLEAN).value_.boolean_)

BooleanType::BooleanType() : Type(TypeId::BOOLEAN) {}

CmpBool BooleanType::CompareEquals(const Value &left, const Value &right) const {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(==);  // NOLINT
}

CmpBool BooleanType::CompareNotEquals(const Value &left, const Value &right) const {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(!=);  // NOLINT
}

CmpBool BooleanType::CompareLessThan(const Value &left, const Value &right) const {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(<);  // NOLINT
}

CmpBool BooleanType::CompareLessThanEquals(const Value &left, const Value &right) const {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(<=);  // NOLINT
}

CmpBool BooleanType::CompareGreaterThan(const Value &left, const Value &right) const {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(>);  // NOLINT
}

CmpBool BooleanType::CompareGreaterThanEquals(const Value &left, const Value &right) const {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(>=);  // NOLINT
}

std::string BooleanType::ToString(const Value &val) const {
  assert(GetTypeId() == TypeId::BOOLEAN);
  if (val.value_.boolean_ == 1) {
    return "true";
  }
  if (val.value_.boolean_ == 0) {
    return "false";
  }
  return "boolean_null";
}

void BooleanType::SerializeTo(const Value &val, char *storage) const {
  *reinterpret_cast<int8_t *>(storage) = val.value_.boolean_;
}

// Deserialize a value of the given type from the given storage space.
Value BooleanType::DeserializeFrom(const char *storage) const {
  int8_t val = *reinterpret_cast<const int8_t *>(storage);
  return Value(TypeId::BOOLEAN, val);
}

Value BooleanType::Copy(const Value &val) const { return Value(TypeId::BOOLEAN, val.value_.boolean_); }

Value BooleanType::CastAs(const Value &val, const TypeId type_id) const {
  switch (type_id) {
    case TypeId::BOOLEAN:
      return Copy(val);
    case TypeId::VARCHAR: {
      if (val.IsNull()) {
        return Value(TypeId::VARCHAR, nullptr, 0, false);
      }
      return Value(TypeId::VARCHAR, val.ToString());
    }
    default:
      break;
  }
  throw Exception("BOOLEAN is not coercable to " + Type::TypeIdToString(type_id));
}
}  // namespace bustub
