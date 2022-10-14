#include "binder/statement/index_statement.h"
#include "binder/bound_expression.h"
#include "binder/expressions/bound_column_ref.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace bustub {

IndexStatement::IndexStatement(std::string index_name, std::unique_ptr<BoundBaseTableRef> table,
                               std::vector<std::unique_ptr<BoundColumnRef>> cols)
    : BoundStatement(StatementType::INDEX_STATEMENT),
      index_name_(std::move(index_name)),
      table_(std::move(table)),
      cols_(std::move(cols)) {}

auto IndexStatement::ToString() const -> std::string {
  return fmt::format("BoundIndex {{ index_name={}, table={}, cols={} }}", index_name_, *table_, cols_);
}

}  // namespace bustub
