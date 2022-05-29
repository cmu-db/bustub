//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// varlen_type.h
//
// Identification: src/include/type/varlen_type.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <string>
#include "type/value.h"

namespace bustub {
/* A varlen value is an abstract class representing all objects that have
 * variable length.
 * For simplicity, for valen_type we always set flag "inline" as true, which
 * means we store actual data along with its size rather than a pointer
 */
class VarlenType : public Type {
 public:
  explicit VarlenType(TypeId type);
  ~VarlenType() override;

  // Access the raw variable length data
  auto GetData(const Value &val) const -> const char * override;

  // Get the length of the variable length data
  auto GetLength(const Value &val) const -> uint32_t override;

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

  // Debug
  auto ToString(const Value &val) const -> std::string override;

  // Serialize this value into the given storage space
  void SerializeTo(const Value &val, char *storage) const override;

  // Deserialize a value of the given type from the given storage space.
  auto DeserializeFrom(const char *storage) const -> Value override;

  // Create a copy of this value
  auto Copy(const Value &val) const -> Value override;
};
}  // namespace bustub
