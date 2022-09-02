//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// statement_type.h
//
// Identification: src/include/enums/statement_type.h
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "common/config.h"
#include "fmt/core.h"

namespace bustub {

//===--------------------------------------------------------------------===//
// Statement Types
//===--------------------------------------------------------------------===//
enum class StatementType : uint8_t {
  INVALID_STATEMENT,       // invalid statement type
  SELECT_STATEMENT,        // select statement type
  INSERT_STATEMENT,        // insert statement type
  UPDATE_STATEMENT,        // update statement type
  CREATE_STATEMENT,        // create statement type
  DELETE_STATEMENT,        // delete statement type
  PREPARE_STATEMENT,       // prepare statement type
  EXECUTE_STATEMENT,       // execute statement type
  ALTER_STATEMENT,         // alter statement type
  TRANSACTION_STATEMENT,   // transaction statement type,
  COPY_STATEMENT,          // copy type
  ANALYZE_STATEMENT,       // analyze type
  VARIABLE_SET_STATEMENT,  // variable set statement type
  CREATE_FUNC_STATEMENT,   // create func statement type
  EXPLAIN_STATEMENT,       // explain statement type
  DROP_STATEMENT,          // DROP statement type
  EXPORT_STATEMENT,        // EXPORT statement type
  PRAGMA_STATEMENT,        // PRAGMA statement type
  SHOW_STATEMENT,          // SHOW statement type
  VACUUM_STATEMENT,        // VACUUM statement type
  CALL_STATEMENT,          // CALL statement type
  SET_STATEMENT,           // SET statement type
  LOAD_STATEMENT,          // LOAD statement type
  RELATION_STATEMENT
};

}  // namespace bustub

template <>
struct fmt::formatter<bustub::StatementType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::StatementType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::StatementType::INVALID_STATEMENT:
        name = "Invalid";
        break;
      case bustub::StatementType::SELECT_STATEMENT:
        name = "Select";
        break;
      case bustub::StatementType::INSERT_STATEMENT:
        name = "Insert";
        break;
      case bustub::StatementType::UPDATE_STATEMENT:
        name = "Update";
        break;
      case bustub::StatementType::CREATE_STATEMENT:
        name = "Create";
        break;
      case bustub::StatementType::DELETE_STATEMENT:
        name = "Delete";
        break;
      case bustub::StatementType::EXPLAIN_STATEMENT:
        name = "Explain";
        break;
      case bustub::StatementType::DROP_STATEMENT:
        name = "Drop";
        break;
      default:
        name = "Unknown";
    }
    return formatter<string_view>::format(name, ctx);
  }
};
