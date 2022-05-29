//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// decimal_type.h
//
// Identification: src/include/type/decimal_type.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <string>
#include "type/numeric_type.h"

namespace bustub {
class DecimalType : public NumericType {
 public:
  DecimalType();
  // DecimalValue(DecDef definition);

  // Other mathematical functions
  auto Add(const Value &left, const Value &right) const -> Value override;
  auto Subtract(const Value &left, const Value &right) const -> Value override;
  auto Multiply(const Value &left, const Value &right) const -> Value override;
  auto Divide(const Value &left, const Value &right) const -> Value override;
  auto Modulo(const Value &left, const Value &right) const -> Value override;
  auto Min(const Value &left, const Value &right) const -> Value override;
  auto Max(const Value &left, const Value &right) const -> Value override;
  auto Sqrt(const Value &val) const -> Value override;
  auto IsZero(const Value &val) const -> bool override;

  // Comparison functions
  auto CompareEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareNotEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareLessThan(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareLessThanEquals(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareGreaterThan(const Value &left, const Value &right) const -> CmpBool override;
  auto CompareGreaterThanEquals(const Value &left, const Value &right) const -> CmpBool override;

  auto CastAs(const Value &val, TypeId type_id) const -> Value override;

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

 private:
  auto OperateNull(const Value &left, const Value &right) const -> Value override;
};
}  // namespace bustub
