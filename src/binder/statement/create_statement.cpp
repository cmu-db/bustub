//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// create_statement.cpp
//
// Identification: src/binder/statement/create_statement.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "catalog/column.h"
#include "fmt/ranges.h"

#include "binder/statement/create_statement.h"

namespace bustub {

CreateStatement::CreateStatement(std::string table, std::vector<Column> columns, std::vector<std::string> primary_key)
    : BoundStatement(StatementType::CREATE_STATEMENT),
      table_(std::move(table)),
      columns_(std::move(columns)),
      primary_key_(std::move(primary_key)) {}

auto CreateStatement::ToString() const -> std::string {
  return fmt::format("BoundCreate {{\n  table={}\n  columns={}\n  primary_key={}\n}}", table_, columns_, primary_key_);
}

}  // namespace bustub
