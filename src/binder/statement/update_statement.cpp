//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// update_statement.cpp
//
// Identification: src/binder/statement/update_statement.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "fmt/ranges.h"

#include "binder/statement/update_statement.h"

namespace bustub {

UpdateStatement::UpdateStatement(
    std::unique_ptr<BoundBaseTableRef> table, std::unique_ptr<BoundExpression> filter_expr,
    std::vector<std::pair<std::unique_ptr<BoundColumnRef>, std::unique_ptr<BoundExpression>>> target_expr)
    : BoundStatement(StatementType::UPDATE_STATEMENT),
      table_(std::move(table)),
      filter_expr_(std::move(filter_expr)),
      target_expr_(std::move(target_expr)) {}

auto UpdateStatement::ToString() const -> std::string {
  return fmt::format("BoundUpdate {{\n  table={},\n  filter_expr={},\n  target_expr={},\n}}", *table_, *filter_expr_,
                     target_expr_);
}

}  // namespace bustub
