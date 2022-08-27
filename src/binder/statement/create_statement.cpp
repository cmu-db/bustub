#include "binder/statement/create_statement.h"

namespace bustub {

CreateStatement::CreateStatement(const Parser &parser, duckdb_libpgquery::PGCreateStmt *pg_stmt)
    : SQLStatement(StatementType::CREATE_STATEMENT) {
  table_ = pg_stmt->relation->relname;

  if (pg_stmt->tableElts == nullptr) {
    throw Exception("Table must have at least one column!");
  }

  for (auto c = pg_stmt->tableElts->head; c != nullptr; c = lnext(c)) {
    auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
    switch (node->type) {
      case duckdb_libpgquery::T_PGColumnDef: {
        auto cdef = static_cast<duckdb_libpgquery::PGColumnDef *>(c->data.ptr_value);
        auto centry = parser.TransformColumnDefinition(cdef);
        columns_.push_back(move(centry));
        break;
      }
      default:
        throw NotImplementedException("ColumnDef type not handled yet");
    }
  }
}

auto CreateStatement::ToString() const -> std::string { return {}; }

}  // namespace bustub
