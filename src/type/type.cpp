//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// type.cpp
//
// Identification: src/type/type.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <string>
#include "common/exception.h"
#include "type/bigint_type.h"
#include "type/boolean_type.h"
#include "type/decimal_type.h"
#include "type/integer_type.h"
#include "type/smallint_type.h"
#include "type/tinyint_type.h"
#include "type/value.h"
#include "type/varlen_type.h"

namespace bustub {

Type *Type::k_types[] = {
    new Type(TypeId::INVALID),        new BooleanType(), new TinyintType(), new SmallintType(),
    new IntegerType(TypeId::INTEGER), new BigintType(),  new DecimalType(), new VarlenType(TypeId::VARCHAR),
};

// Get the size of this data type in bytes
auto Type::GetTypeSize(const TypeId type_id) -> uint64_t {
  switch (type_id) {
    case BOOLEAN:
    case TINYINT:
      return 1;
    case SMALLINT:
      return 2;
    case INTEGER:
      return 4;
    case BIGINT:
    case DECIMAL:
    case TIMESTAMP:
      return 8;
    case VARCHAR:
      return 0;
    default:
      break;
  }
  throw Exception(ExceptionType::UNKNOWN_TYPE, "Unknown type.");
}

auto Type::IsCoercableFrom(const TypeId type_id) const -> bool {
  switch (type_id_) {
    case INVALID:
      return false;
    case BOOLEAN:
      return true;
    case TINYINT:
    case SMALLINT:
    case INTEGER:
    case BIGINT:
    case DECIMAL:
      switch (type_id) {
        case TINYINT:
        case SMALLINT:
        case INTEGER:
        case BIGINT:
        case DECIMAL:
        case VARCHAR:
          return true;
        default:
          return false;
      }
      break;
    case TIMESTAMP:
      return (type_id == VARCHAR || type_id == TIMESTAMP);
    case VARCHAR:
      switch (type_id) {
        case BOOLEAN:
        case TINYINT:
        case SMALLINT:
        case INTEGER:
        case BIGINT:
        case DECIMAL:
        case TIMESTAMP:
        case VARCHAR:
          return true;
        default:
          return false;
      }
      break;
    default:
      return (type_id == type_id_);
  }  // END SWITCH
}

auto Type::TypeIdToString(const TypeId type_id) -> std::string {
  switch (type_id) {
    case INVALID:
      return "INVALID";
    case BOOLEAN:
      return "BOOLEAN";
    case TINYINT:
      return "TINYINT";
    case SMALLINT:
      return "SMALLINT";
    case INTEGER:
      return "INTEGER";
    case BIGINT:
      return "BIGINT";
    case DECIMAL:
      return "DECIMAL";
    case TIMESTAMP:
      return "TIMESTAMP";
    case VARCHAR:
      return "VARCHAR";
    default:
      return "INVALID";
  }
}

auto Type::GetMinValue(TypeId type_id) -> Value {
  switch (type_id) {
    case BOOLEAN:
      return {type_id, 0};
    case TINYINT:
      return {type_id, BUSTUB_INT8_MIN};
    case SMALLINT:
      return {type_id, BUSTUB_INT16_MIN};
    case INTEGER:
      return {type_id, BUSTUB_INT32_MIN};
    case BIGINT:
      return {type_id, BUSTUB_INT64_MIN};
    case DECIMAL:
      return {type_id, BUSTUB_DECIMAL_MIN};
    case TIMESTAMP:
      return {type_id, 0};
    case VARCHAR:
      return {type_id, ""};
    default:
      break;
  }
  throw Exception(ExceptionType::MISMATCH_TYPE, "Cannot get minimal value.");
}

auto Type::GetMaxValue(TypeId type_id) -> Value {
  switch (type_id) {
    case BOOLEAN:
      return {type_id, 1};
    case TINYINT:
      return {type_id, BUSTUB_INT8_MAX};
    case SMALLINT:
      return {type_id, BUSTUB_INT16_MAX};
    case INTEGER:
      return {type_id, BUSTUB_INT32_MAX};
    case BIGINT:
      return {type_id, BUSTUB_INT64_MAX};
    case DECIMAL:
      return {type_id, BUSTUB_DECIMAL_MAX};
    case TIMESTAMP:
      return {type_id, BUSTUB_TIMESTAMP_MAX};
    case VARCHAR:
      return {type_id, nullptr, 0, false};
    default:
      break;
  }
  throw Exception(ExceptionType::MISMATCH_TYPE, "Cannot get max value.");
}

auto Type::CompareEquals(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const
    -> CmpBool {
  throw NotImplementedException("CompareEquals not implemented");
}

auto Type::CompareNotEquals(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const
    -> CmpBool {
  throw NotImplementedException("CompareNotEquals not implemented");
}

auto Type::CompareLessThan(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const
    -> CmpBool {
  throw NotImplementedException("CompareLessThan not implemented");
}
auto Type::CompareLessThanEquals(const Value &left __attribute__((unused)),
                                 const Value &right __attribute__((unused))) const -> CmpBool {
  throw NotImplementedException("CompareLessThanEqual not implemented");
}
auto Type::CompareGreaterThan(const Value &left __attribute__((unused)),
                              const Value &right __attribute__((unused))) const -> CmpBool {
  throw NotImplementedException("CompareGreaterThan not implemented");
}
auto Type::CompareGreaterThanEquals(const Value &left __attribute__((unused)),
                                    const Value &right __attribute__((unused))) const -> CmpBool {
  throw NotImplementedException("CompareGreaterThanEqual not implemented");
}

// Other mathematical functions
auto Type::Add(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const -> Value {
  throw NotImplementedException("Add not implemented");
}

auto Type::Subtract(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const
    -> Value {
  throw NotImplementedException("Subtract not implemented");
}

auto Type::Multiply(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const
    -> Value {
  throw NotImplementedException("Multiply not implemented");
}

auto Type::Divide(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const
    -> Value {
  throw NotImplementedException("Divide not implemented");
}

auto Type::Modulo(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const
    -> Value {
  throw NotImplementedException("Modulo not implemented");
}

auto Type::Min(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const -> Value {
  throw NotImplementedException("Min not implemented");
}

auto Type::Max(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const -> Value {
  throw NotImplementedException("Max not implemented");
}

auto Type::Sqrt(const Value &val __attribute__((unused))) const -> Value {
  throw NotImplementedException("Sqrt not implemented");
}

auto Type::OperateNull(const Value &val __attribute__((unused)), const Value &right __attribute__((unused))) const
    -> Value {
  throw NotImplementedException("OperateNull not implemented");
}

auto Type::IsZero(const Value &val __attribute__((unused))) const -> bool {
  throw NotImplementedException("isZero not implemented");
}
// Is the data inlined into this classes storage space, or must it be accessed
// through an indirection/pointer?
auto Type::IsInlined(const Value &val __attribute__((unused))) const -> bool {
  throw NotImplementedException("IsLined not implemented");
}

// Return a stringified version of this value
auto Type::ToString(const Value &val __attribute__((unused))) const -> std::string {
  throw NotImplementedException("ToString not implemented");
}

// Serialize this value into the given storage space. The inlined parameter
// indicates whether we are allowed to inline this value into the storage
// space, or whether we must store only a reference to this value. If inlined
// is false, we may use the provided data pool to allocate space for this
// value, storing a reference into the allocated pool space in the storage.
void Type::SerializeTo(const Value &val __attribute__((unused)), char *storage __attribute__((unused))) const {
  throw NotImplementedException("SerializeTo not implemented");
}

// Deserialize a value of the given type from the given storage space.
auto Type::DeserializeFrom(const char *storage __attribute__((unused))) const -> Value {
  throw NotImplementedException("DeserializeFrom not implemented");
}

// Create a copy of this value
auto Type::Copy(const Value &val __attribute__((unused))) const -> Value {
  throw NotImplementedException("Copy not implemented");
}

auto Type::CastAs(const Value &val __attribute__((unused)), const TypeId type_id __attribute__((unused))) const
    -> Value {
  throw NotImplementedException("CastAs not implemented");
}

// Access the raw variable length data
auto Type::GetData(const Value &val __attribute__((unused))) const -> const char * {
  throw NotImplementedException("GetData from value not implemented");
}

// Get the length of the variable length data
auto Type::GetLength(const Value &val __attribute__((unused))) const -> uint32_t {
  throw NotImplementedException("GetLength not implemented");
}

// Access the raw varlen data stored from the tuple storage
auto Type::GetData(char *storage __attribute__((unused))) -> char * {
  throw NotImplementedException("GetData not implemented");
}

}  // namespace bustub
