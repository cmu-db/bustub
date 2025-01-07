//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// column.cpp
//
// Identification: src/catalog/column.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "catalog/column.h"

#include <sstream>
#include <string>
#include "type/type_id.h"

namespace bustub {

/** @return a string representation of this column */
auto Column::ToString(bool simplified) const -> std::string {
  if (simplified) {
    std::ostringstream os;
    os << column_name_ << ":" << Type::TypeIdToString(column_type_);
    if (column_type_ == VARCHAR) {
      os << "(" << length_ << ")";
    }
    if (column_type_ == VECTOR) {
      os << "(" << length_ / sizeof(double) << ")";
    }
    return (os.str());
  }

  std::ostringstream os;

  os << "Column[" << column_name_ << ", " << Type::TypeIdToString(column_type_) << ", "
     << "Offset:" << column_offset_ << ", ";
  os << "Length:" << length_;
  os << "]";
  return (os.str());
}

}  // namespace bustub
