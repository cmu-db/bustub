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

 public:
  /** Render this statement as a string. */
  virtual auto ToString() const -> std::string {
    throw Exception("ToString not supported for this type of SQLStatement");
  }
};

}  // namespace bustub
