//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// timestamp_type.h
//
// Identification: src/include/type/timestamp_type.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include "type/abstract_pool.h"
#include "type/value.h"

namespace bustub {

class TimestampType : public Type {
 public:
  static constexpr uint64_t K_USECS_PER_DATE = 86400000000UL;

  ~TimestampType() override = default;
  TimestampType();

  // Comparison functions
  CmpBool CompareEquals(const Value &left, const Value &right) const override;
  CmpBool CompareNotEquals(const Value &left, const Value &right) const override;
  CmpBool CompareLessThan(const Value &left, const Value &right) const override;
  CmpBool CompareLessThanEquals(const Value &left, const Value &right) const override;
  CmpBool CompareGreaterThan(const Value &left, const Value &right) const override;
  CmpBool CompareGreaterThanEquals(const Value &left, const Value &right) const override;

  // Other mathematical functions
  Value Min(const Value &left, const Value &right) const override;
  Value Max(const Value &left, const Value &right) const override;

  bool IsInlined(const Value &val) const override { return true; }

  // Debug
  std::string ToString(const Value &val) const override;

  // Serialize this value into the given storage space
  void SerializeTo(const Value &val, char *storage) const override;

  // Deserialize a value of the given type from the given storage space.
  Value DeserializeFrom(const char *storage) const override;

  // Create a copy of this value
  Value Copy(const Value &val) const override;

  Value CastAs(const Value &val, TypeId type_id) const override;
};

}  // namespace bustub
