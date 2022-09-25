#include "fmt/ranges.h"

#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_table_ref.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "common/util/string_util.h"

namespace bustub {

InsertStatement::InsertStatement(std::unique_ptr<BoundBaseTableRef> table, std::unique_ptr<SelectStatement> select)
    : BoundStatement(StatementType::INSERT_STATEMENT), table_(std::move(table)), select_(std::move(select)) {}

auto InsertStatement::ToString() const -> std::string {
  return fmt::format("BoundInsert {{\n  table={},\n  select={}\n}}", *table_,
                     StringUtil::IndentAllLines(select_->ToString(), 2));
}

}  // namespace bustub
