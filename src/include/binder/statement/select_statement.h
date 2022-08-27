//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/select_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

#include "binder/parser.h"

namespace bustub {

class SelectStatement : public SQLStatement {
 public:
  explicit SelectStatement(const Parser &parser, duckdb_libpgquery::PGSelectStmt *pg_stmt);

  string table_;
  vector<Column> columns_;

  auto ToString() const -> string override;
};

}  // namespace bustub
