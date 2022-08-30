//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/insert_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

#include "binder/binder.h"

namespace bustub {

class InsertStatement : public SQLStatement {
 public:
  explicit InsertStatement(duckdb_libpgquery::PGInsertStmt *pg_stmt);

  string table_;
  vector<Column> columns_;
  vector<vector<Value>> values_;

  auto ToString() const -> string override;
};

}  // namespace bustub
