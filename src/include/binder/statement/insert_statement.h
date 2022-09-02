//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/insert_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

#include "binder/bound_statement.h"
#include "catalog/column.h"
#include "type/value.h"

namespace duckdb_libpgquery {
struct PGInsertStmt;
}  // namespace duckdb_libpgquery

namespace bustub {

class InsertStatement : public BoundStatement {
 public:
  explicit InsertStatement(duckdb_libpgquery::PGInsertStmt *pg_stmt);

  std::string table_;
  std::vector<Column> columns_;
  std::vector<std::vector<Value>> values_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
