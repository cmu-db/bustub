//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// tinyint_type.h
//
// Identification: src/include/type/tinyint_type.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once
#include <string>
#include "type/integer_parent_type.h"

namespace bustub {
// An integer value of the common sizes.
class TinyintType : public IntegerParentType {
 public:
  ~TinyintType() override = default;

  TinyintType();

  // Other mathematical functions
  Value Add(const Value &left, const Value &right) const override;
  Value Subtract(const Value &left, const Value &right) const override;
  Value Multiply(const Value &left, const Value &right) const override;
  Value Divide(const Value &left, const Value &right) const override;
  Value Modulo(const Value &left, const Value &right) const override;
  Value Sqrt(const Value &val) const override;

  // Comparison functions
  CmpBool CompareEquals(const Value &left, const Value &right) const override;
  CmpBool CompareNotEquals(const Value &left, const Value &right) const override;
  CmpBool CompareLessThan(const Value &left, const Value &right) const override;
  CmpBool CompareLessThanEquals(const Value &left, const Value &right) const override;
  CmpBool CompareGreaterThan(const Value &left, const Value &right) const override;
  CmpBool CompareGreaterThanEquals(const Value &left, const Value &right) const override;

  Value CastAs(const Value &val, TypeId type_id) const override;

  // Debug
  std::string ToString(const Value &val) const override;

  // Serialize this value into the given storage space
  void SerializeTo(const Value &val, char *storage) const override;

  // Deserialize a value of the given type from the given storage space.
  Value DeserializeFrom(const char *storage) const override;

  // Create a copy of this value
  Value Copy(const Value &val) const override;

 protected:
  Value OperateNull(const Value &left, const Value &right) const override;

  bool IsZero(const Value &val) const override;
};
}  // namespace bustub
