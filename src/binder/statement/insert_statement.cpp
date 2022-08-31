#include "binder/statement/insert_statement.h"

namespace bustub {

InsertStatement::InsertStatement(duckdb_libpgquery::PGInsertStmt *pg_stmt)
    : BoundStatement(StatementType::INSERT_STATEMENT) {}

auto InsertStatement::ToString() const -> std::string { return {}; }

}  // namespace bustub
