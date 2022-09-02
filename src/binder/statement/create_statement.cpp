#include "binder/statement/create_statement.h"

namespace bustub {

CreateStatement::CreateStatement(duckdb_libpgquery::PGCreateStmt *pg_stmt)
    : BoundStatement(StatementType::CREATE_STATEMENT) {}

auto CreateStatement::ToString() const -> std::string { return {}; }

}  // namespace bustub
