//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// boolean_type.cpp
//
// Identification: src/type/boolean_type.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>

#include <string>
#include "type/boolean_type.h"

namespace bustub {
#define BOOLEAN_COMPARE_FUNC(OP) GetCmpBool(left.value_.boolean_ OP right.CastAs(TypeId::BOOLEAN).value_.boolean_)

BooleanType::BooleanType() : Type(TypeId::BOOLEAN) {}

auto BooleanType::CompareEquals(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(==);  // NOLINT
}

auto BooleanType::CompareNotEquals(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(!=);  // NOLINT
}

auto BooleanType::CompareLessThan(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(<);  // NOLINT
}

auto BooleanType::CompareLessThanEquals(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(<=);  // NOLINT
}

auto BooleanType::CompareGreaterThan(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(>);  // NOLINT
}

auto BooleanType::CompareGreaterThanEquals(const Value &left, const Value &right) const -> CmpBool {
  assert(GetTypeId() == TypeId::BOOLEAN);
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return BOOLEAN_COMPARE_FUNC(>=);  // NOLINT
}

/**
 * Debug
 */
auto BooleanType::ToString(const Value &val) const -> std::string {
  assert(GetTypeId() == TypeId::BOOLEAN);
  if (val.value_.boolean_ == 1) {
    return "true";
  }
  if (val.value_.boolean_ == 0) {
    return "false";
  }
  return "boolean_null";
}

/**
 * Serialize this value into the given storage space
 */
void BooleanType::SerializeTo(const Value &val, char *storage) const {
  *reinterpret_cast<int8_t *>(storage) = val.value_.boolean_;
}

/**
 * Deserialize a value of the given type from the given storage space.
 */
auto BooleanType::DeserializeFrom(const char *storage) const -> Value {
  int8_t val = *reinterpret_cast<const int8_t *>(storage);
  return {TypeId::BOOLEAN, val};
}

/**
 * Create a copy of this value
 */
auto BooleanType::Copy(const Value &val) const -> Value { return {TypeId::BOOLEAN, val.value_.boolean_}; }

auto BooleanType::CastAs(const Value &val, const TypeId type_id) const -> Value {
  switch (type_id) {
    case TypeId::BOOLEAN:
      return Copy(val);
    case TypeId::VARCHAR: {
      if (val.IsNull()) {
        return {TypeId::VARCHAR, nullptr, 0, false};
      }
      return {TypeId::VARCHAR, val.ToString()};
    }
    default:
      break;
  }
  throw Exception("BOOLEAN is not coercable to " + Type::TypeIdToString(type_id));
}
}  // namespace bustub
