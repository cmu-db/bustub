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

  std::string table_;
  std::vector<Column> columns_;
  std::vector<std::vector<Value>> values_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
