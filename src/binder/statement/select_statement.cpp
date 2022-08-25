#include "binder/statement/select_statement.h"
#include <fmt/format.h>

namespace bustub {

SelectStatement::SelectStatement(const Parser &parser, duckdb_libpgquery::PGSelectStmt *pg_stmt)
    : SQLStatement(StatementType::SELECT_STATEMENT) {
  bool found = false;

  // Extract the table name from the FROM clause.
  for (auto c = pg_stmt->fromClause->head; c != nullptr; c = lnext(c)) {
    auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
    switch (node->type) {
      case duckdb_libpgquery::T_PGRangeVar: {
        if (found) {
          throw Exception("shouldnt have multiple tables");
        }
        auto *table_ref = reinterpret_cast<duckdb_libpgquery::PGRangeVar *>(node);
        table_ = table_ref->relname;
        found = true;
        break;
      }
      default:
        throw Exception("Found unknown node type");
    }
  }
}

std::string SelectStatement::ToString() const {
  std::vector<std::string> columns;
  for (auto &column : columns_) {
    columns.push_back(fmt::format("{}", column.ToString()));
  }
  return fmt::format("Select {{ table={}, columns={} }}", table_, fmt::join(columns, ", "));
}

}  // namespace bustub
