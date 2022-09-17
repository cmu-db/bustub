#include <memory>

#include "binder/bound_order_by.h"
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
                                 std::unique_ptr<BoundExpression> having, std::unique_ptr<BoundExpression> limit_count,
                                 std::unique_ptr<BoundExpression> limit_offset,
                                 std::vector<std::unique_ptr<BoundOrderBy>> sort)
    : BoundStatement(StatementType::SELECT_STATEMENT),
      table_(std::move(table)),
      select_list_(std::move(select_list)),
      where_(std::move(where)),
      group_by_(std::move(group_by)),
      having_(std::move(having)),
      limit_count_(std::move(limit_count)),
      limit_offset_(std::move(limit_offset)),
      sort_(std::move(sort)) {}

auto SelectStatement::ToString() const -> std::string {
  return fmt::format(
      "BoundSelect {{\n  table={},\n  columns={},\n  groupBy={},\n  having={},\n  where={},\n  limit={},\n  "
      "offset={},\n  order_by={}}}",
      table_, select_list_, group_by_, having_, where_, limit_count_, limit_offset_, sort_);
}
}  // namespace bustub
