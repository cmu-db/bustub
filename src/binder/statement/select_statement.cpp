#include <memory>

#include "fmt/format.h"
#include "fmt/ranges.h"

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/statement/select_statement.h"

namespace bustub {

SelectStatement::SelectStatement(std::unique_ptr<BoundTableRef> table,
                                 std::vector<std::unique_ptr<BoundExpression>> select_list,
                                 std::unique_ptr<BoundExpression> where,
                                 std::vector<std::unique_ptr<BoundExpression>> group_by,
                                 std::unique_ptr<BoundExpression> having)
    : BoundStatement(StatementType::SELECT_STATEMENT),
      table_(std::move(table)),
      select_list_(std::move(select_list)),
      where_(std::move(where)),
      group_by_(std::move(group_by)),
      having_(std::move(having)) {}

auto SelectStatement::ToString() const -> std::string {
  return fmt::format("BoundSelect {{\n  table={},\n  columns={},\n  groupBy={},\n  having={},\n  where={}\n}}", table_,
                     select_list_, group_by_, having_, where_);
}
}  // namespace bustub
