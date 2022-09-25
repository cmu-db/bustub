#include <iterator>
#include <memory>
#include <string>

#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_table_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "nodes/parsenodes.hpp"
#include "type/value_factory.h"

namespace bustub {

auto Binder::BindInsert(duckdb_libpgquery::PGInsertStmt *pg_stmt) -> std::unique_ptr<InsertStatement> {
  if (pg_stmt->cols != nullptr) {
    throw NotImplementedException("insert only supports all columns, don't specify columns");
  }

  auto table = BindRangeVar(pg_stmt->relation);

  if (StringUtil::StartsWith(table->table_, "__")) {
    throw bustub::Exception(fmt::format("invalid table for insert: {}", table->table_));
  }

  auto select_statement = BindSelect(reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(pg_stmt->selectStmt));

  return std::make_unique<InsertStatement>(std::move(table), std::move(select_statement));
}

auto Binder::BindDelete(duckdb_libpgquery::PGDeleteStmt *stmt) -> std::unique_ptr<DeleteStatement> {
  auto table = BindRangeVar(stmt->relation);
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

}  // namespace bustub
