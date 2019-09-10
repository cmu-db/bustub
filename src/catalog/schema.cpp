//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// schema.cpp
//
// Identification: src/catalog/schema.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "catalog/schema.h"

#include <sstream>
#include <string>
#include <vector>

namespace bustub {

Schema::Schema(const std::vector<Column> &columns) : tuple_is_inlined_(true) {
  uint32_t curr_offset = 0;
  for (uint32_t index = 0; index < columns.size(); index++) {
    Column column = columns[index];
    // handle uninlined column
    if (!column.IsInlined()) {
      tuple_is_inlined_ = false;
      uninlined_columns_.push_back(index);
    }
    // set column offset
    column.column_offset_ = curr_offset;
    curr_offset += column.GetFixedLength();

    // add column
    this->columns_.push_back(column);
  }
  // set tuple length
  length_ = curr_offset;
}

std::string Schema::ToString() const {
  std::ostringstream os;

  os << "Schema["
     << "NumColumns:" << GetColumnCount() << ", "
     << "IsInlined:" << tuple_is_inlined_ << ", "
     << "Length:" << length_ << "]";

  bool first = true;
  os << " :: (";
  for (uint32_t i = 0; i < GetColumnCount(); i++) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    os << columns_[i].ToString();
  }
  os << ")";

  return os.str();
}

}  // namespace bustub
