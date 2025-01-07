//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// generic_key.h
//
// Identification: src/include/storage/index/generic_key.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstring>

#include "storage/table/tuple.h"
#include "type/value.h"

namespace bustub {

/**
 * Generic key is used for indexing with opaque data.
 *
 * This key type uses an fixed length array to hold data for indexing
 * purposes, the actual size of which is specified and instantiated
 * with a template argument.
 */
template <size_t KeySize>
class GenericKey {
 public:
  inline void SetFromKey(const Tuple &tuple) {
    // initialize to 0
    memset(data_, 0, KeySize);
    memcpy(data_, tuple.GetData(), tuple.GetLength());
  }

  // NOTE: for test purpose only
  inline void SetFromInteger(int64_t key) {
    memset(data_, 0, KeySize);
    memcpy(data_, &key, sizeof(int64_t));
  }

  inline auto ToValue(Schema *schema, uint32_t column_idx) const -> Value {
    const char *data_ptr;
    const auto &col = schema->GetColumn(column_idx);
    const TypeId column_type = col.GetType();
    const bool is_inlined = col.IsInlined();
    if (is_inlined) {
      data_ptr = (data_ + col.GetOffset());
    } else {
      int32_t offset = *reinterpret_cast<int32_t *>(const_cast<char *>(data_ + col.GetOffset()));
      data_ptr = (data_ + offset);
    }
    return Value::DeserializeFrom(data_ptr, column_type);
  }

  // NOTE: for test purpose only
  // interpret the first 8 bytes as int64_t from data vector
  inline auto ToString() const -> int64_t { return *reinterpret_cast<int64_t *>(const_cast<char *>(data_)); }

  // NOTE: for test purpose only
  // interpret the first 8 bytes as int64_t from data vector
  friend auto operator<<(std::ostream &os, const GenericKey &key) -> std::ostream & {
    os << key.ToString();
    return os;
  }

  // actual location of data, extends past the end.
  char data_[KeySize];
};

/**
 * Function object returns true if lhs < rhs, used for trees
 */
template <size_t KeySize>
class GenericComparator {
 public:
  inline auto operator()(const GenericKey<KeySize> &lhs, const GenericKey<KeySize> &rhs) const -> int {
    uint32_t column_count = key_schema_->GetColumnCount();

    for (uint32_t i = 0; i < column_count; i++) {
      Value lhs_value = (lhs.ToValue(key_schema_, i));
      Value rhs_value = (rhs.ToValue(key_schema_, i));

      if (lhs_value.CompareLessThan(rhs_value) == CmpBool::CmpTrue) {
        return -1;
      }
      if (lhs_value.CompareGreaterThan(rhs_value) == CmpBool::CmpTrue) {
        return 1;
      }
    }
    // equals
    return 0;
  }

  GenericComparator(const GenericComparator &other) : key_schema_{other.key_schema_} {}

  // constructor
  explicit GenericComparator(Schema *key_schema) : key_schema_(key_schema) {}

 private:
  Schema *key_schema_;
};

}  // namespace bustub
