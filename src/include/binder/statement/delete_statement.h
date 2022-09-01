//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/delete_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>

#include "binder/binder.h"

namespace bustub {

class DeleteStatement : public SQLStatement {
 public:
  explicit DeleteStatement(duckdb_libpgquery::PGDeleteStmt *pg_stmt);

  std::string table_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
