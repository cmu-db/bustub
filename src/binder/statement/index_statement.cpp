//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// index_statement.cpp
//
// Identification: src/binder/statement/index_statement.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "binder/statement/index_statement.h"
#include "binder/bound_expression.h"
#include "binder/expressions/bound_column_ref.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace bustub {

IndexStatement::IndexStatement(std::string index_name, std::unique_ptr<BoundBaseTableRef> table,
                               std::vector<std::unique_ptr<BoundColumnRef>> cols, std::string index_type,
                               std::vector<std::string> col_options, std::vector<std::pair<std::string, int>> options)
    : BoundStatement(StatementType::INDEX_STATEMENT),
      index_name_(std::move(index_name)),
      table_(std::move(table)),
      cols_(std::move(cols)),
      index_type_(std::move(index_type)),
      col_options_(std::move(col_options)),
      options_(std::move(options)) {}

auto IndexStatement::ToString() const -> std::string {
  return fmt::format("BoundIndex {{ index_name={}, table={}, cols={}, using={}, col_options=[{}], options=[{}] }}",
                     index_name_, *table_, cols_, index_type_, fmt::join(col_options_, ","), fmt::join(options_, ","));
}

}  // namespace bustub
