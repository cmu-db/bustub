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
#include "binder/statement/copy_statement.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/statement/update_statement.h"
#include "binder/tokens.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "nodes/parsenodes.hpp"
#include "type/value_factory.h"

namespace bustub {

auto Binder::BindCopy(duckdb_libpgquery::PGCopyStmt *pg_stmt) -> std::unique_ptr<CopyStatement> {
  if (pg_stmt->is_from) {
    return BindCopyFrom(pg_stmt);
  }
  return BindCopyTo(pg_stmt);
}

auto Binder::BindCopyFrom(duckdb_libpgquery::PGCopyStmt *pg_stmt) -> std::unique_ptr<CopyStatement> {
  auto table = BindBaseTableRef(pg_stmt->relation->relname, std::nullopt);

  std::vector<std::unique_ptr<BoundColumnRef>> exprs;
  if (pg_stmt->attlist != nullptr) {
    for (auto col_node = pg_stmt->attlist->head; col_node != nullptr; col_node = col_node->next) {
      auto target = reinterpret_cast<duckdb_libpgquery::PGResTarget *>(col_node->data.ptr_value);
      if (target->name != nullptr) {
        auto column = ResolveColumnRefFromBaseTableRef(*table, std::vector{std::string{target->name}});
        exprs.emplace_back(std::move(column));
      }
    }
  }
  auto res = std::make_unique<CopyStatement>(std::move(table), std::move(exprs));
  res->is_from_ = true;
  if (StringUtil::EndsWith(pg_stmt->filename, ".csv")) {
    res->SetCSVFormat();
  }
  res->file_path_ = pg_stmt->filename;
  return res;
}

auto Binder::BindCopyTo(duckdb_libpgquery::PGCopyStmt *pg_stmt) -> std::unique_ptr<CopyStatement> {
  // SelectNode: use pg_stmt->query
  throw NotImplementedException("copyTo() not implemented");
}
};  // namespace bustub