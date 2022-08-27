//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/create_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

#include "binder/parser.h"

namespace bustub {

class CreateStatement : public SQLStatement {
 public:
  explicit CreateStatement(const Parser &parser, duckdb_libpgquery::PGCreateStmt *pg_stmt);

  string table_;
  vector<Column> columns_;

  auto ToString() const -> string override;
};

}  // namespace bustub
