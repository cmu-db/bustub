//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// boolean_type.h
//
// Identification: src/include/type/boolean_type.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <string>
#include "common/exception.h"
#include "type/type.h"
#include "type/value.h"

namespace bustub {
// A boolean value isn't a real SQL type, but we treat it as one to keep
// consistent in the expression subsystem.
class BooleanType : public Type {
 public:
  ~BooleanType() override = default;
  BooleanType();

  // Comparison functions
  auto CompareEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareNotEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareLessThan(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareLessThanEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareGreaterThan(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareGreaterThanEquals(const Value &left, const Value &right) const -> CmpBool override;

  // Decimal types are always inlined
  auto IsInlined(const Value &val) const -> bool override { return true; }

  // Debug
  auto ToString(const Value &val) const -> std::string override;

  // Serialize this value into the given storage space
  void SerializeTo(const Value &val, char *storage) const override;

  // Deserialize a value of the given type from the given storage space.
  auto DeserializeFrom(const char *storage) const -> Value override;

  // Create a copy of this value
  auto Copy(const Value &val) const -> Value override;

  auto CastAs(const Value &val, TypeId type_id) const -> Value override;
};
}  // namespace bustub
