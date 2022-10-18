//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// tuple.cpp
//
// Identification: src/storage/table/tuple.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#include "storage/table/tuple.h"

namespace bustub {

// TODO(Amadou): It does not look like nulls are supported. Add a null bitmap?
Tuple::Tuple(std::vector<Value> values, const Schema *schema) : allocated_(true) {
  assert(values.size() == schema->GetColumnCount());

  // 1. Calculate the size of the tuple.
  uint32_t tuple_size = schema->GetLength();
  for (auto &i : schema->GetUnlinedColumns()) {
    auto len = values[i].GetLength();
    if (len == BUSTUB_VALUE_NULL) {
      len = 0;
    }
    tuple_size += (len + sizeof(uint32_t));
  }

  // 2. Allocate memory.
  size_ = tuple_size;
  data_ = new char[size_];
  std::memset(data_, 0, size_);

  // 3. Serialize each attribute based on the input value.
  uint32_t column_count = schema->GetColumnCount();
  uint32_t offset = schema->GetLength();

  for (uint32_t i = 0; i < column_count; i++) {
    const auto &col = schema->GetColumn(i);
    if (!col.IsInlined()) {
      // Serialize relative offset, where the actual varchar data is stored.
      *reinterpret_cast<uint32_t *>(data_ + col.GetOffset()) = offset;
      // Serialize varchar value, in place (size+data).
      values[i].SerializeTo(data_ + offset);
      auto len = values[i].GetLength();
      if (len == BUSTUB_VALUE_NULL) {
        len = 0;
      }
      offset += (len + sizeof(uint32_t));
    } else {
      values[i].SerializeTo(data_ + col.GetOffset());
    }
  }
}

Tuple::Tuple(const Tuple &other) : allocated_(other.allocated_), rid_(other.rid_), size_(other.size_) {
  if (allocated_) {
    delete[] data_;
  }
  if (allocated_) {
    // Deep copy.
    data_ = new char[size_];
    memcpy(data_, other.data_, size_);
  } else {
    // Shallow copy.
    data_ = other.data_;
  }
}

auto Tuple::operator=(const Tuple &other) -> Tuple & {
  if (allocated_) {
    delete[] data_;
  }
  allocated_ = other.allocated_;
  rid_ = other.rid_;
  size_ = other.size_;

  if (allocated_) {
    // Deep copy.
    data_ = new char[size_];
    memcpy(data_, other.data_, size_);
  } else {
    // Shallow copy.
    data_ = other.data_;
  }

  return *this;
}

auto Tuple::GetValue(const Schema *schema, const uint32_t column_idx) const -> Value {
  assert(schema);
  assert(data_);
  const TypeId column_type = schema->GetColumn(column_idx).GetType();
  const char *data_ptr = GetDataPtr(schema, column_idx);
  // the third parameter "is_inlined" is unused
  return Value::DeserializeFrom(data_ptr, column_type);
}

auto Tuple::KeyFromTuple(const Schema &schema, const Schema &key_schema, const std::vector<uint32_t> &key_attrs)
    -> Tuple {
  std::vector<Value> values;
  values.reserve(key_attrs.size());
  for (auto idx : key_attrs) {
    values.emplace_back(this->GetValue(&schema, idx));
  }
  return {values, &key_schema};
}

auto Tuple::GetDataPtr(const Schema *schema, const uint32_t column_idx) const -> const char * {
  assert(schema);
  assert(data_);
  const auto &col = schema->GetColumn(column_idx);
  bool is_inlined = col.IsInlined();
  // For inline type, data is stored where it is.
  if (is_inlined) {
    return (data_ + col.GetOffset());
  }
  // We read the relative offset from the tuple data.
  int32_t offset = *reinterpret_cast<int32_t *>(data_ + col.GetOffset());
  // And return the beginning address of the real data for the VARCHAR type.
  return (data_ + offset);
}

auto Tuple::ToString(const Schema *schema) const -> std::string {
  std::stringstream os;

  int column_count = schema->GetColumnCount();
  bool first = true;
  os << "(";
  for (int column_itr = 0; column_itr < column_count; column_itr++) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    if (IsNull(schema, column_itr)) {
      os << "<NULL>";
    } else {
      Value val = (GetValue(schema, column_itr));
      os << val.ToString();
    }
  }
  os << ")";
  os << " Tuple size is " << size_;

  return os.str();
}

void Tuple::SerializeTo(char *storage) const {
  memcpy(storage, &size_, sizeof(int32_t));
  memcpy(storage + sizeof(int32_t), data_, size_);
}

void Tuple::DeserializeFrom(const char *storage) {
  uint32_t size = *reinterpret_cast<const uint32_t *>(storage);
  // Construct a tuple.
  this->size_ = size;
  if (this->allocated_) {
    delete[] this->data_;
  }
  this->data_ = new char[this->size_];
  memcpy(this->data_, storage + sizeof(int32_t), this->size_);
  this->allocated_ = true;
}

}  // namespace bustub
