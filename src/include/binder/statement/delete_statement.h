//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/delete_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>

#include "binder/parser.h"

namespace bustub {

class DeleteStatement : public SQLStatement {
 public:
  explicit DeleteStatement(const Parser &parser, duckdb_libpgquery::PGDeleteStmt *pg_stmt);

  string table_;

  auto ToString() const -> string override;
};

}  // namespace bustub
