//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// column.cpp
//
// Identification: src/catalog/column.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "catalog/column.h"

#include <sstream>
#include <string>

namespace bustub {

std::string Column::ToString() const {
  std::ostringstream os;

  os << "Column[" << column_name_ << ", " << Type::TypeIdToString(column_type_) << ", "
     << "Offset:" << column_offset_ << ", ";

  if (IsInlined()) {
    os << "FixedLength:" << fixed_length_;
  } else {
    os << "VarLength:" << variable_length_;
  }
  os << "]";
  return (os.str());
}

}  // namespace bustub
