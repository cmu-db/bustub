//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// create_statement.h
//
// Identification: src/include/binder/statement/create_statement.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

#include "binder/bound_statement.h"
#include "catalog/column.h"

namespace duckdb_libpgquery {
struct PGCreateStmt;
}  // namespace duckdb_libpgquery

namespace bustub {

class CreateStatement : public BoundStatement {
 public:
  explicit CreateStatement(std::string table, std::vector<Column> columns, std::vector<std::string> primary_key);

  std::string table_;
  std::vector<Column> columns_;
  std::vector<std::string> primary_key_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
