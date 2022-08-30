//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/create_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

#include "binder/binder.h"

namespace bustub {

class CreateStatement : public SQLStatement {
 public:
  explicit CreateStatement(duckdb_libpgquery::PGCreateStmt *pg_stmt);

  string table_;
  vector<Column> columns_;

  auto ToString() const -> string override;
};

}  // namespace bustub
