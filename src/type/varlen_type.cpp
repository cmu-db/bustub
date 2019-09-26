//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// varlen_type.cpp
//
// Identification: src/type/varlen_type.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <string>

#include "common/exception.h"
#include "type/type_util.h"
#include "type/varlen_type.h"

namespace bustub {
#define VARLEN_COMPARE_FUNC(OP)                                               \
  const char *str1 = left.GetData();                                          \
  uint32_t len1 = GetLength(left) - 1;                                        \
  const char *str2;                                                           \
  uint32_t len2;                                                              \
  if (right.GetTypeId() == TypeId::VARCHAR) {                                 \
    str2 = right.GetData();                                                   \
    len2 = GetLength(right) - 1;                                              \
    /* NOLINTNEXTLINE */                                                      \
    return GetCmpBool(TypeUtil::CompareStrings(str1, len1, str2, len2) OP 0); \
  } else {                                                                    \
    auto r_value = right.CastAs(TypeId::VARCHAR);                             \
    str2 = r_value.GetData();                                                 \
    len2 = GetLength(r_value) - 1;                                            \
    /* NOLINTNEXTLINE */                                                      \
    return GetCmpBool(TypeUtil::CompareStrings(str1, len1, str2, len2) OP 0); \
  }

VarlenType::VarlenType(TypeId type) : Type(type) {}

VarlenType::~VarlenType() = default;

// Access the raw variable length data
const char *VarlenType::GetData(const Value &val) const { return val.value_.varlen_; }

// Get the length of the variable length data (including the length field)
uint32_t VarlenType::GetLength(const Value &val) const { return val.size_.len_; }

CmpBool VarlenType::CompareEquals(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  if (GetLength(left) == BUSTUB_VARCHAR_MAX_LEN || GetLength(right) == BUSTUB_VARCHAR_MAX_LEN) {
    return GetCmpBool(GetLength(left) == GetLength(right));
  }

  VARLEN_COMPARE_FUNC(==);  // NOLINT
}

CmpBool VarlenType::CompareNotEquals(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  if (GetLength(left) == BUSTUB_VARCHAR_MAX_LEN || GetLength(right) == BUSTUB_VARCHAR_MAX_LEN) {
    return GetCmpBool(GetLength(left) != GetLength(right));
  }

  VARLEN_COMPARE_FUNC(!=);  // NOLINT
}

CmpBool VarlenType::CompareLessThan(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  if (GetLength(left) == BUSTUB_VARCHAR_MAX_LEN || GetLength(right) == BUSTUB_VARCHAR_MAX_LEN) {
    return GetCmpBool(GetLength(left) < GetLength(right));
  }

  VARLEN_COMPARE_FUNC(<);  // NOLINT
}

CmpBool VarlenType::CompareLessThanEquals(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  if (GetLength(left) == BUSTUB_VARCHAR_MAX_LEN || GetLength(right) == BUSTUB_VARCHAR_MAX_LEN) {
    return GetCmpBool(GetLength(left) <= GetLength(right));
  }

  VARLEN_COMPARE_FUNC(<=);  // NOLINT
}

CmpBool VarlenType::CompareGreaterThan(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  if (GetLength(left) == BUSTUB_VARCHAR_MAX_LEN || GetLength(right) == BUSTUB_VARCHAR_MAX_LEN) {
    return GetCmpBool(GetLength(left) > GetLength(right));
  }

  VARLEN_COMPARE_FUNC(>);  // NOLINT
}

CmpBool VarlenType::CompareGreaterThanEquals(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return CmpBool::CmpNull;
  }
  if (GetLength(left) == BUSTUB_VARCHAR_MAX_LEN || GetLength(right) == BUSTUB_VARCHAR_MAX_LEN) {
    return GetCmpBool(GetLength(left) >= GetLength(right));
  }

  VARLEN_COMPARE_FUNC(>=);  // NOLINT
}

Value VarlenType::Min(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }
  if (left.CompareLessThan(right) == CmpBool::CmpTrue) {
    return left.Copy();
  }
  return right.Copy();
}

Value VarlenType::Max(const Value &left, const Value &right) const {
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }
  if (left.CompareGreaterThan(right) == CmpBool::CmpTrue) {
    return left.Copy();
  }
  return right.Copy();
}

std::string VarlenType::ToString(const Value &val) const {
  uint32_t len = GetLength(val);

  if (val.IsNull()) {
    return "varlen_null";
  }
  if (len == BUSTUB_VARCHAR_MAX_LEN) {
    return "varlen_max";
  }
  if (len == 0) {
    return "";
  }
  return std::string(GetData(val), len - 1);
}

void VarlenType::SerializeTo(const Value &val, char *storage) const {
  uint32_t len = GetLength(val);
  if (len == BUSTUB_VALUE_NULL) {
    memcpy(storage, &len, sizeof(uint32_t));
    return;
  }
  memcpy(storage, &len, sizeof(uint32_t));
  memcpy(storage + sizeof(uint32_t), val.value_.varlen_, len);
}

// Deserialize a value of the given type from the given storage space.
Value VarlenType::DeserializeFrom(const char *storage) const {
  uint32_t len = *reinterpret_cast<const uint32_t *>(storage);
  if (len == BUSTUB_VALUE_NULL) {
    return Value(type_id_, nullptr, len, false);
  }
  // set manage_data as true
  return Value(type_id_, storage + sizeof(uint32_t), len, true);
}

Value VarlenType::Copy(const Value &val) const { return Value(val); }

Value VarlenType::CastAs(const Value &value, const TypeId type_id) const {
  std::string str;
  // switch begins
  switch (type_id) {
    case TypeId::BOOLEAN: {
      str = value.ToString();
      std::transform(str.begin(), str.end(), str.begin(), ::tolower);
      if (str == "true" || str == "1" || str == "t") {
        return Value(type_id, 1);
      }
      if (str == "false" || str == "0" || str == "f") {
        return Value(type_id, 0);
      }
      throw Exception("Boolean value format error.");
    }
    case TypeId::TINYINT: {
      str = value.ToString();
      int8_t tinyint = 0;
      try {
        tinyint = static_cast<int8_t>(stoi(str));
      } catch (std::out_of_range &e) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      if (tinyint < BUSTUB_INT8_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return Value(type_id, tinyint);
    }
    case TypeId::SMALLINT: {
      str = value.ToString();
      int16_t smallint = 0;
      try {
        smallint = static_cast<int16_t>(stoi(str));
      } catch (std::out_of_range &e) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      if (smallint < BUSTUB_INT16_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return Value(type_id, smallint);
    }
    case TypeId::INTEGER: {
      str = value.ToString();
      int32_t integer = 0;
      try {
        integer = stoi(str);
      } catch (std::out_of_range &e) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      if (integer > BUSTUB_INT32_MAX || integer < BUSTUB_INT32_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return Value(type_id, integer);
    }
    case TypeId::BIGINT: {
      str = value.ToString();
      int64_t bigint = 0;
      try {
        bigint = stoll(str);
      } catch (std::out_of_range &e) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      if (bigint > BUSTUB_INT64_MAX || bigint < BUSTUB_INT64_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return Value(type_id, bigint);
    }
    case TypeId::DECIMAL: {
      str = value.ToString();
      double res = 0;
      try {
        res = stod(str);
      } catch (std::out_of_range &e) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      if (res > BUSTUB_DECIMAL_MAX || res < BUSTUB_DECIMAL_MIN) {
        throw Exception(ExceptionType::OUT_OF_RANGE, "Numeric value out of range.");
      }
      return Value(type_id, res);
    }
    case TypeId::VARCHAR:
      return value.Copy();
    default:
      break;
  }
  throw Exception("VARCHAR is not coercable to " + TypeIdToString(type_id));
}
}  // namespace bustub
