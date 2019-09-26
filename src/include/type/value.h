//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// value.h
//
// Identification: src/include/type/value.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstring>
#include <string>
#include <utility>

#include "type/limits.h"
#include "type/type.h"

namespace bustub {

inline CmpBool GetCmpBool(bool boolean) { return boolean ? CmpBool::CmpTrue : CmpBool::CmpFalse; }

// A value is an abstract class that represents a view over SQL data stored in
// some materialized state. All values have a type and comparison functions, but
// subclasses implement other type-specific functionality.
class Value {
  // Friend Type classes
  friend class Type;
  friend class NumericType;
  friend class IntegerParentType;
  friend class TinyintType;
  friend class SmallintType;
  friend class IntegerType;
  friend class BigintType;
  friend class DecimalType;
  friend class TimestampType;
  friend class BooleanType;
  friend class VarlenType;

 public:
  explicit Value(const TypeId type) : manage_data_(false), type_id_(type) { size_.len_ = BUSTUB_VALUE_NULL; }
  // BOOLEAN and TINYINT
  Value(TypeId type, int8_t i);
  // DECIMAL
  Value(TypeId type, double d);
  Value(TypeId type, float f);
  // SMALLINT
  Value(TypeId type, int16_t i);
  // INTEGER
  Value(TypeId type, int32_t i);
  // BIGINT
  Value(TypeId type, int64_t i);
  // TIMESTAMP
  Value(TypeId type, uint64_t i);
  // VARCHAR
  Value(TypeId type, const char *data, uint32_t len, bool manage_data);
  Value(TypeId type, const std::string &data);

  Value() : Value(TypeId::INVALID) {}
  Value(const Value &other);
  Value &operator=(Value other);
  ~Value();
  // NOLINTNEXTLINE
  friend void Swap(Value &first, Value &second) {
    std::swap(first.value_, second.value_);
    std::swap(first.size_, second.size_);
    std::swap(first.manage_data_, second.manage_data_);
    std::swap(first.type_id_, second.type_id_);
  }
  // check whether value is integer
  bool CheckInteger() const;
  bool CheckComparable(const Value &o) const;

  // Get the type of this value
  inline TypeId GetTypeId() const { return type_id_; }

  // Get the length of the variable length data
  inline uint32_t GetLength() const { return Type::GetInstance(type_id_)->GetLength(*this); }
  // Access the raw variable length data
  inline const char *GetData() const { return Type::GetInstance(type_id_)->GetData(*this); }

  template <class T>
  inline T GetAs() const {
    return *reinterpret_cast<const T *>(&value_);
  }

  inline Value CastAs(const TypeId type_id) const { return Type::GetInstance(type_id_)->CastAs(*this, type_id); }
  // Comparison Methods
  inline CmpBool CompareEquals(const Value &o) const { return Type::GetInstance(type_id_)->CompareEquals(*this, o); }
  inline CmpBool CompareNotEquals(const Value &o) const {
    return Type::GetInstance(type_id_)->CompareNotEquals(*this, o);
  }
  inline CmpBool CompareLessThan(const Value &o) const {
    return Type::GetInstance(type_id_)->CompareLessThan(*this, o);
  }
  inline CmpBool CompareLessThanEquals(const Value &o) const {
    return Type::GetInstance(type_id_)->CompareLessThanEquals(*this, o);
  }
  inline CmpBool CompareGreaterThan(const Value &o) const {
    return Type::GetInstance(type_id_)->CompareGreaterThan(*this, o);
  }
  inline CmpBool CompareGreaterThanEquals(const Value &o) const {
    return Type::GetInstance(type_id_)->CompareGreaterThanEquals(*this, o);
  }

  // Other mathematical functions
  inline Value Add(const Value &o) const { return Type::GetInstance(type_id_)->Add(*this, o); }
  inline Value Subtract(const Value &o) const { return Type::GetInstance(type_id_)->Subtract(*this, o); }
  inline Value Multiply(const Value &o) const { return Type::GetInstance(type_id_)->Multiply(*this, o); }
  inline Value Divide(const Value &o) const { return Type::GetInstance(type_id_)->Divide(*this, o); }
  inline Value Modulo(const Value &o) const { return Type::GetInstance(type_id_)->Modulo(*this, o); }
  inline Value Min(const Value &o) const { return Type::GetInstance(type_id_)->Min(*this, o); }
  inline Value Max(const Value &o) const { return Type::GetInstance(type_id_)->Max(*this, o); }
  inline Value Sqrt() const { return Type::GetInstance(type_id_)->Sqrt(*this); }

  inline Value OperateNull(const Value &o) const { return Type::GetInstance(type_id_)->OperateNull(*this, o); }
  inline bool IsZero() const { return Type::GetInstance(type_id_)->IsZero(*this); }
  inline bool IsNull() const { return size_.len_ == BUSTUB_VALUE_NULL; }

  // Serialize this value into the given storage space. The inlined parameter
  // indicates whether we are allowed to inline this value into the storage
  // space, or whether we must store only a reference to this value. If inlined
  // is false, we may use the provided data pool to allocate space for this
  // value, storing a reference into the allocated pool space in the storage.
  inline void SerializeTo(char *storage) const { Type::GetInstance(type_id_)->SerializeTo(*this, storage); }

  // Deserialize a value of the given type from the given storage space.
  inline static Value DeserializeFrom(const char *storage, const TypeId type_id) {
    return Type::GetInstance(type_id)->DeserializeFrom(storage);
  }

  // Return a string version of this value
  inline std::string ToString() const { return Type::GetInstance(type_id_)->ToString(*this); }
  // Create a copy of this value
  inline Value Copy() const { return Type::GetInstance(type_id_)->Copy(*this); }

 protected:
  // The actual value item
  union Val {
    int8_t boolean_;
    int8_t tinyint_;
    int16_t smallint_;
    int32_t integer_;
    int64_t bigint_;
    double decimal_;
    uint64_t timestamp_;
    char *varlen_;
    const char *const_varlen_;
  } value_;

  union {
    uint32_t len_;
    TypeId elem_type_id_;
  } size_;

  bool manage_data_;
  // The data type
  TypeId type_id_;
};
}  // namespace bustub
