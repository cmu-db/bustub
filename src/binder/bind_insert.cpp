//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bind_insert.cpp
//
// Identification: src/binder/bind_insert.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <iterator>
#include <memory>
#include <optional>
#include <string>

#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_table_ref.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/statement/update_statement.h"
#include "binder/tokens.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "nodes/parsenodes.hpp"
#include "type/value_factory.h"

namespace bustub {

auto Binder::BindInsert(duckdb_libpgquery::PGInsertStmt *pg_stmt) -> std::unique_ptr<InsertStatement> {
  if (pg_stmt->cols != nullptr) {
    throw NotImplementedException("insert only supports all columns, don't specify columns");
  }

  auto table = BindBaseTableRef(pg_stmt->relation->relname, std::nullopt);

  if (StringUtil::StartsWith(table->table_, "__")) {
    throw bustub::Exception(fmt::format("invalid table for insert: {}", table->table_));
  }

  auto select_statement = BindSelect(reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(pg_stmt->selectStmt));

  return std::make_unique<InsertStatement>(std::move(table), std::move(select_statement));
}

auto Binder::BindDelete(duckdb_libpgquery::PGDeleteStmt *stmt) -> std::unique_ptr<DeleteStatement> {
  auto table = BindBaseTableRef(stmt->relation->relname, std::nullopt);
  auto ctx_guard = NewContext();
  scope_ = table.get();
  std::unique_ptr<BoundExpression> expr = nullptr;
  if (stmt->whereClause != nullptr) {
    expr = BindExpression(stmt->whereClause);
  } else {
    expr = std::make_unique<BoundConstant>(ValueFactory::GetBooleanValue(true));
  }

  return std::make_unique<DeleteStatement>(std::move(table), std::move(expr));
}

auto Binder::BindUpdate(duckdb_libpgquery::PGUpdateStmt *stmt) -> std::unique_ptr<UpdateStatement> {
  if (stmt->withClause != nullptr) {
    throw bustub::NotImplementedException("update with clause not supported yet");
  }

  if (stmt->fromClause != nullptr) {
    throw bustub::NotImplementedException("update from clause not supported yet");
  }

  auto table = BindBaseTableRef(stmt->relation->relname, std::nullopt);
  auto ctx_guard = NewContext();
  scope_ = table.get();

  std::unique_ptr<BoundExpression> filter_expr = nullptr;

  if (stmt->whereClause != nullptr) {
    filter_expr = BindExpression(stmt->whereClause);
  } else {
    filter_expr = std::make_unique<BoundConstant>(ValueFactory::GetBooleanValue(true));
  }

  auto root = stmt->targetList;
  std::vector<std::pair<std::unique_ptr<BoundColumnRef>, std::unique_ptr<BoundExpression>>> target_expr;

  for (auto cell = root->head; cell != nullptr; cell = cell->next) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGResTarget *>(cell->data.ptr_value);
    auto column = ResolveColumnRefFromBaseTableRef(*table, std::vector{std::string{target->name}});
    target_expr.emplace_back(std::make_pair(std::move(column), BindExpression(target->val)));
  }

  return std::make_unique<UpdateStatement>(std::move(table), std::move(filter_expr), std::move(target_expr));
}

}  // namespace bustub
