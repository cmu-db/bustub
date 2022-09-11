#include "catalog/column.h"
#include "fmt/ranges.h"

#include "binder/statement/create_statement.h"

namespace bustub {

CreateStatement::CreateStatement(std::string table, std::vector<Column> columns)
    : BoundStatement(StatementType::CREATE_STATEMENT), table_(std::move(table)), columns_(std::move(columns)) {}

auto CreateStatement::ToString() const -> std::string {
  return fmt::format("BoundCreate {{\n  table={}\n  columns={}\n}}", table_, columns_);
}

}  // namespace bustub
