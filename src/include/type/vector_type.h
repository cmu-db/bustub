//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// vector_type.h
//
// Identification: src/include/type/vector_type.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <string>
#include <valarray>
#include <vector>
#include "type/value.h"

namespace bustub {
/* A varlen value is an abstract class representing all objects that have
 * variable length.
 * For simplicity, for valen_type we always set flag "inline" as true, which
 * means we store actual data along with its size rather than a pointer
 */
class VectorType : public Type {
 public:
  VectorType();
  ~VectorType() override;

  auto GetData(const Value &val) const -> const char * override;

  auto GetVector(const Value &val) const -> std::vector<double>;

  auto GetStorageSize(const Value &val) const -> uint32_t override;

  // Comparison functions
  auto CompareEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareNotEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareLessThan(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareLessThanEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareGreaterThan(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareGreaterThanEquals(const Value &left, const Value &right) const -> CmpBool override;

  // Other mathematical functions
  auto Min(const Value &left, const Value &right) const -> Value override;
  auto Max(const Value &left, const Value &right) const -> Value override;

  auto CastAs(const Value &value, TypeId type_id) const -> Value override;

  // Decimal types are always inlined
  auto IsInlined(const Value & /*val*/) const -> bool override { return false; }

  auto ToString(const Value &val) const -> std::string override;

  void SerializeTo(const Value &val, char *storage) const override;

  auto DeserializeFrom(const char *storage) const -> Value override;

  auto Copy(const Value &val) const -> Value override;
};
}  // namespace bustub
