//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/insert_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

#include "binder/parser.h"

namespace bustub {

class InsertStatement : public SQLStatement {
 public:
  explicit InsertStatement(const Parser &parser, duckdb_libpgquery::PGInsertStmt *pg_stmt);

  string table_;
  vector<Column> columns_;
  vector<vector<Value>> values_;

  string ToString() const override;
};

}  // namespace bustub
