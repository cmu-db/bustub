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

  std::string table_;
  std::vector<Column> columns_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
