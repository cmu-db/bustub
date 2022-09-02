//===----------------------------------------------------------------------===//
//                         BusTub
//
// bustub/binder/sql_statement.h
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>

#include "common/enums/statement_type.h"
#include "common/exception.h"

namespace bustub {

/**
 * BoundStatement is the base class of any type of bound SQL statement.
 */
class BoundStatement {
 public:
  explicit BoundStatement(StatementType type);
  virtual ~BoundStatement() = default;

  /** The statement type. */
  StatementType type_;

  /** The statement location within the query string. */
  int32_t stmt_location_;

  /** The statement length within the query string. */
  int32_t stmt_length_;

  /** The query text that corresponds to this SQL statement. */
  std::string query_;

 public:
  /** Render this statement as a string. */
  virtual auto ToString() const -> std::string {
    throw Exception("ToString not supported for this type of SQLStatement");
  }
};

}  // namespace bustub
