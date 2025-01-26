//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// vector_type.cpp
//
// Identification: src/type/vector_type.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <string>

#include "common/exception.h"
#include "common/macros.h"
#include "fmt/ranges.h"
#include "type/type_id.h"
#include "type/type_util.h"
#include "type/vector_type.h"

namespace bustub {

VectorType::VectorType() : Type(TypeId::VECTOR) {}

VectorType::~VectorType() = default;

/**
 * Access the raw variable length data
 */
auto VectorType::GetData(const Value &val) const -> const char * { return val.value_.varlen_; }

auto VectorType::GetVector(const Value &val) const -> std::vector<double> {
  auto *base_ptr = reinterpret_cast<double *>(val.value_.varlen_);
  auto size = val.size_.len_ / sizeof(double);
  std::vector<double> data;
  data.reserve(size);
  for (unsigned i = 0; i < size; i++) {
    data.push_back(base_ptr[i]);
  }
  return data;
}

/**
 * Get the length of the variable length data (including the length field)
 */
auto VectorType::GetStorageSize(const Value &val) const -> uint32_t { return val.size_.len_; }

auto VectorType::CompareEquals(const Value &left, const Value &right) const -> CmpBool {
  UNIMPLEMENTED("vector type comparison not supported");
}

auto VectorType::CompareNotEquals(const Value &left, const Value &right) const -> CmpBool {
  UNIMPLEMENTED("vector type comparison not supported");
}

auto VectorType::CompareLessThan(const Value &left, const Value &right) const -> CmpBool {
  UNIMPLEMENTED("vector type comparison not supported");
}

auto VectorType::CompareLessThanEquals(const Value &left, const Value &right) const -> CmpBool {
  UNIMPLEMENTED("vector type comparison not supported");
}

auto VectorType::CompareGreaterThan(const Value &left, const Value &right) const -> CmpBool {
  UNIMPLEMENTED("vector type comparison not supported");
}

auto VectorType::CompareGreaterThanEquals(const Value &left, const Value &right) const -> CmpBool {
  UNIMPLEMENTED("vector type comparison not supported");
}

auto VectorType::Min(const Value &left, const Value &right) const -> Value {
  UNIMPLEMENTED("vector type comparison not supported");
}

auto VectorType::Max(const Value &left, const Value &right) const -> Value {
  UNIMPLEMENTED("vector type comparison not supported");
}

/**
 * Debug
 */
auto VectorType::ToString(const Value &val) const -> std::string {
  uint32_t len = GetStorageSize(val);

  if (val.IsNull()) {
    return "vector_null";
  }
  if (len == BUSTUB_VARCHAR_MAX_LEN) {
    return "vector_max";
  }
  if (len == 0) {
    return "";
  }
  return fmt::format("[{}]", fmt::join(GetVector(val), ","));
}

/**
 * Serialize this value into the given storage space
 */
void VectorType::SerializeTo(const Value &val, char *storage) const {
  uint32_t len = GetStorageSize(val);
  if (len == BUSTUB_VALUE_NULL) {
    memcpy(storage, &len, sizeof(uint32_t));
    return;
  }
  memcpy(storage, &len, sizeof(uint32_t));
  memcpy(storage + sizeof(uint32_t), val.value_.varlen_, len);
}

/**
 * Deserialize a value of the given type from the given storage space.
 */
auto VectorType::DeserializeFrom(const char *storage) const -> Value {
  uint32_t len = *reinterpret_cast<const uint32_t *>(storage);
  if (len == BUSTUB_VALUE_NULL) {
    return {type_id_, nullptr, len, false};
  }
  // set manage_data as true
  return {type_id_, storage + sizeof(uint32_t), len, true};
}

/**
 * Create a copy of this value
 */
auto VectorType::Copy(const Value &val) const -> Value { return {val}; }

auto VectorType::CastAs(const Value &value, const TypeId type_id) const -> Value {
  UNIMPLEMENTED("vector type cast not supported");
}
}  // namespace bustub
