#include <memory>

#include "binder/bound_order_by.h"
#include "common/util/string_util.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/statement/select_statement.h"

namespace bustub {

auto SelectStatement::ToString() const -> std::string {
  return fmt::format(
      "BoundSelect {{\n  table={},\n  columns={},\n  groupBy={},\n  having={},\n  where={},\n  limit={},\n  "
      "offset={},\n  order_by={},\n  is_distinct={},\n  ctes={},\n}}",
      StringUtil::IndentAllLines(table_->ToString(), 2, true), select_list_, group_by_, having_, where_, limit_count_,
      limit_offset_, sort_, is_distinct_,
      StringUtil::IndentAllLines(fmt::format("{}", fmt::join(ctes_, ",\n")), 2, true));
}

}  // namespace bustub
