#include "binder/statement/insert_statement.h"

namespace bustub {

InsertStatement::InsertStatement(const Parser &parser, duckdb_libpgquery::PGInsertStmt *pg_stmt)
    : SQLStatement(StatementType::INSERT_STATEMENT) {
  table_ = pg_stmt->relation->relname;

  vector<string> col_names;
  // resolve the insertion column names
  if (pg_stmt->cols != nullptr) {
    for (auto c = pg_stmt->cols->head; c != nullptr; c = lnext(c)) {
      auto target = static_cast<duckdb_libpgquery::PGResTarget *>(c->data.ptr_value);
      col_names.emplace_back(target->name);
    }
  }

  // Now resolve the column types using the catalog.

  // resolve the insertion values
  auto *select = reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(pg_stmt->selectStmt);

  for (auto value_list = select->valuesLists->head; value_list != nullptr; value_list = value_list->next) {
    auto target = static_cast<duckdb_libpgquery::PGList *>(value_list->data.ptr_value);
    vector<Value> curr_values = parser.TransformExpressionList(target);

    if (!values_.empty() && values_[0].size() != curr_values.size()) {
      throw Exception("VALUES lists must all be the same length");
    }
    values_.push_back(curr_values);
  }
}

auto InsertStatement::ToString() const -> std::string { return {}; }

}  // namespace bustub
