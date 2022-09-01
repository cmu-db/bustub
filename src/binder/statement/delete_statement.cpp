#include "binder/statement/delete_statement.h"

namespace bustub {

DeleteStatement::DeleteStatement(duckdb_libpgquery::PGDeleteStmt *pg_stmt)
    : SQLStatement(StatementType::DELETE_STATEMENT) {}

auto DeleteStatement::ToString() const -> std::string { return {}; }

}  // namespace bustub
