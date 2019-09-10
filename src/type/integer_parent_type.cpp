//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// integer_parent_type.cpp
//
// Identification: src/type/integer_parent_type.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <cmath>

#include "type/integer_parent_type.h"

namespace bustub {
IntegerParentType::IntegerParentType(TypeId type) : NumericType(type) {}

Value IntegerParentType::Min(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  if (left.CompareLessThan(right) == CmpBool::CmpTrue) {
    return left.Copy();
  }
  return right.Copy();
}

Value IntegerParentType::Max(const Value &left, const Value &right) const {
  assert(left.CheckInteger());
  assert(left.CheckComparable(right));
  if (left.IsNull() || right.IsNull()) {
    return left.OperateNull(right);
  }

  if (left.CompareGreaterThanEquals(right) == CmpBool::CmpTrue) {
    return left.Copy();
  }
  return right.Copy();
}
}  // namespace bustub
