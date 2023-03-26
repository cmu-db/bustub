#include "binder/bound_statement.h"
#include "catalog/column.h"
#include "common/enums/statement_type.h"
#include "fmt/ranges.h"

#include "binder/statement/copy_statement.h"

namespace bustub {

CopyStatement::CopyStatement(std::unique_ptr<BoundBaseTableRef> table,
                             std::vector<std::unique_ptr<BoundColumnRef>> columns)
    : BoundStatement(StatementType::COPY_STATEMENT), table_(std::move(table)), columns_(std::move(columns)) {}

auto CopyStatement::ToString() const -> std::string {
  return fmt::format("BoundCopy {{ table={}, filename={} }}", table_, file_path_);
}

};  // namespace bustub