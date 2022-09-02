//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/delete_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>

#include "binder/bound_statement.h"
#include "catalog/column.h"

namespace duckdb_libpgquery {
struct PGDeleteStmt;
}  // namespace duckdb_libpgquery

namespace bustub {

class DeleteStatement : public BoundStatement {
 public:
  explicit DeleteStatement(duckdb_libpgquery::PGDeleteStmt *pg_stmt);

  std::string table_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
