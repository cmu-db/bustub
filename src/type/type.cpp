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
uint64_t Type::GetTypeSize(const TypeId type_id) {
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

bool Type::IsCoercableFrom(const TypeId type_id) const {
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

std::string Type::TypeIdToString(const TypeId type_id) {
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

Value Type::GetMinValue(TypeId type_id) {
  switch (type_id) {
    case BOOLEAN:
      return Value(type_id, 0);
    case TINYINT:
      return Value(type_id, BUSTUB_INT8_MIN);
    case SMALLINT:
      return Value(type_id, BUSTUB_INT16_MIN);
    case INTEGER:
      return Value(type_id, BUSTUB_INT32_MIN);
    case BIGINT:
      return Value(type_id, BUSTUB_INT64_MIN);
    case DECIMAL:
      return Value(type_id, BUSTUB_DECIMAL_MIN);
    case TIMESTAMP:
      return Value(type_id, 0);
    case VARCHAR:
      return Value(type_id, "");
    default:
      break;
  }
  throw Exception(ExceptionType::MISMATCH_TYPE, "Cannot get minimal value.");
}

Value Type::GetMaxValue(TypeId type_id) {
  switch (type_id) {
    case BOOLEAN:
      return Value(type_id, 1);
    case TINYINT:
      return Value(type_id, BUSTUB_INT8_MAX);
    case SMALLINT:
      return Value(type_id, BUSTUB_INT16_MAX);
    case INTEGER:
      return Value(type_id, BUSTUB_INT32_MAX);
    case BIGINT:
      return Value(type_id, BUSTUB_INT64_MAX);
    case DECIMAL:
      return Value(type_id, BUSTUB_DECIMAL_MAX);
    case TIMESTAMP:
      return Value(type_id, BUSTUB_TIMESTAMP_MAX);
    case VARCHAR:
      return Value(type_id, nullptr, 0, false);
    default:
      break;
  }
  throw Exception(ExceptionType::MISMATCH_TYPE, "Cannot get max value.");
}

CmpBool Type::CompareEquals(const Value &left __attribute__((unused)),
                            const Value &right __attribute__((unused))) const {
  throw NotImplementedException("CompareEquals not implemented");
}

CmpBool Type::CompareNotEquals(const Value &left __attribute__((unused)),
                               const Value &right __attribute__((unused))) const {
  throw NotImplementedException("CompareNotEquals not implemented");
}

CmpBool Type::CompareLessThan(const Value &left __attribute__((unused)),
                              const Value &right __attribute__((unused))) const {
  throw NotImplementedException("CompareLessThan not implemented");
}
CmpBool Type::CompareLessThanEquals(const Value &left __attribute__((unused)),
                                    const Value &right __attribute__((unused))) const {
  throw NotImplementedException("CompareLessThanEqual not implemented");
}
CmpBool Type::CompareGreaterThan(const Value &left __attribute__((unused)),
                                 const Value &right __attribute__((unused))) const {
  throw NotImplementedException("CompareGreaterThan not implemented");
}
CmpBool Type::CompareGreaterThanEquals(const Value &left __attribute__((unused)),
                                       const Value &right __attribute__((unused))) const {
  throw NotImplementedException("CompareGreaterThanEqual not implemented");
}

// Other mathematical functions
Value Type::Add(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const {
  throw NotImplementedException("Add not implemented");
}

Value Type::Subtract(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const {
  throw NotImplementedException("Subtract not implemented");
}

Value Type::Multiply(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const {
  throw NotImplementedException("Multiply not implemented");
}

Value Type::Divide(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const {
  throw NotImplementedException("Divide not implemented");
}

Value Type::Modulo(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const {
  throw NotImplementedException("Modulo not implemented");
}

Value Type::Min(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const {
  throw NotImplementedException("Min not implemented");
}

Value Type::Max(const Value &left __attribute__((unused)), const Value &right __attribute__((unused))) const {
  throw NotImplementedException("Max not implemented");
}

Value Type::Sqrt(const Value &val __attribute__((unused))) const {
  throw NotImplementedException("Sqrt not implemented");
}

Value Type::OperateNull(const Value &val __attribute__((unused)), const Value &right __attribute__((unused))) const {
  throw NotImplementedException("OperateNull not implemented");
}

bool Type::IsZero(const Value &val __attribute__((unused))) const {
  throw NotImplementedException("isZero not implemented");
}
// Is the data inlined into this classes storage space, or must it be accessed
// through an indirection/pointer?
bool Type::IsInlined(const Value &val __attribute__((unused))) const {
  throw NotImplementedException("IsLined not implemented");
}

// Return a stringified version of this value
std::string Type::ToString(const Value &val __attribute__((unused))) const {
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
Value Type::DeserializeFrom(const char *storage __attribute__((unused))) const {
  throw NotImplementedException("DeserializeFrom not implemented");
}

// Create a copy of this value
Value Type::Copy(const Value &val __attribute__((unused))) const {
  throw NotImplementedException("Copy not implemented");
}

Value Type::CastAs(const Value &val __attribute__((unused)), const TypeId type_id __attribute__((unused))) const {
  throw NotImplementedException("CastAs not implemented");
}

// Access the raw variable length data
const char *Type::GetData(const Value &val __attribute__((unused))) const {
  throw NotImplementedException("GetData from value not implemented");
}

// Get the length of the variable length data
uint32_t Type::GetLength(const Value &val __attribute__((unused))) const {
  throw NotImplementedException("GetLength not implemented");
}

// Access the raw varlen data stored from the tuple storage
char *Type::GetData(char *storage __attribute__((unused))) { throw NotImplementedException("GetData not implemented"); }

}  // namespace bustub
