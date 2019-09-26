//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// timestamp_type.cpp
//
// Identification: src/type/timestamp_type.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "type/timestamp_type.h"
#include <string>

#include "type/boolean_type.h"
#include "type/value_factory.h"
#include "type/varlen_type.h"

namespace bustub {

TimestampType::TimestampType() : Type(TypeId::TIMESTAMP) {}

CmpBool TimestampType::CompareEquals(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return GetCmpBool(left.GetAs<uint64_t>() == right.GetAs<uint64_t>());
}

CmpBool TimestampType::CompareNotEquals(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return GetCmpBool(left.GetAs<uint64_t>() != right.GetAs<uint64_t>());
}

CmpBool TimestampType::CompareLessThan(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return GetCmpBool(left.GetAs<uint64_t>() < right.GetAs<uint64_t>());
}

CmpBool TimestampType::CompareLessThanEquals(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return GetCmpBool(left.GetAs<uint64_t>() <= right.GetAs<uint64_t>());
}

CmpBool TimestampType::CompareGreaterThan(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return GetCmpBool(left.GetAs<int64_t>() > right.GetAs<int64_t>());
}

CmpBool TimestampType::CompareGreaterThanEquals(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  return GetCmpBool(left.GetAs<uint64_t>() >= right.GetAs<uint64_t>());
}

Value TimestampType::Min(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }
  if (left.CompareLessThan(right) == CmpBool::CmpTrue) {
    return left.Copy();
  }
  return right.Copy();
}

Value TimestampType::Max(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }
  if (left.CompareGreaterThanEquals(right) == CmpBool::CmpTrue) {
    return left.Copy();
  }
  return right.Copy();
}

// Debug
std::string TimestampType::ToString(const Value &val) const {
  if (val.IsNull()) {
    return "timestamp_null";
  }
  uint64_t tm = val.value_.timestamp_;
  auto micro = static_cast<uint32_t>(tm % 1000000);
  tm /= 1000000;
  auto second = static_cast<uint32_t>(tm % 100000);
  auto sec = static_cast<uint16_t>(second % 60);
  second /= 60;
  auto min = static_cast<uint16_t>(second % 60);
  second /= 60;
  auto hour = static_cast<uint16_t>(second % 24);
  tm /= 100000;
  auto year = static_cast<uint16_t>(tm % 10000);
  tm /= 10000;
  auto tz = static_cast<int>(tm % 27);
  tz -= 12;
  tm /= 27;
  auto day = static_cast<uint16_t>(tm % 32);
  tm /= 32;
  auto month = static_cast<uint16_t>(tm);
  char str[30];
  char zone[5];
  sprintf(str, "%04d-%02d-%02d %02d:%02d:%02d.%06d", year, month, day, hour, min, sec, micro);  // NOLINT
  if (tz >= 0) {
    str[26] = '+';
  } else {
    str[26] = '-';
  }
  if (tz < 0) {
    tz = -tz;
  }
  sprintf(zone, "%02d", tz);  // NOLINT
  str[27] = 0;
  return std::string(std::string(str) + std::string(zone));
}

void TimestampType::SerializeTo(const Value &val, char *storage) const {
  *reinterpret_cast<uint64_t *>(storage) = val.value_.timestamp_;
}

// Deserialize a value of the given type from the given storage space.
Value TimestampType::DeserializeFrom(const char *storage) const {
  uint64_t val = *reinterpret_cast<const uint64_t *>(storage);
  return Value(type_id_, val);
}

// Create a copy of this value
Value TimestampType::Copy(const Value &val) const { return Value(val); }

Value TimestampType::CastAs(const Value &val, const TypeId type_id) const {
  switch (type_id) {
    case TypeId::TIMESTAMP:
      return Copy(val);
    case TypeId::VARCHAR:
      if (val.IsNull()) {
        return ValueFactory::GetVarcharValue(nullptr, false);
      }
      return ValueFactory::GetVarcharValue(val.ToString());
    default:
      break;
  }
  throw Exception("TIMESTAMP is not coercable to " + Type::GetInstance(type_id)->ToString(val));
}

}  // namespace bustub
