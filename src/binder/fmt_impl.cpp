#include "binder/bound_order_by.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/statement/select_statement.h"
#include "binder/table_ref/bound_expression_list_ref.h"
#include "binder/table_ref/bound_subquery_ref.h"
#include "common/util/string_util.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace bustub {

auto BoundAggCall::ToString() const -> std::string {
  if (is_distinct_) {
    return fmt::format("{}_distinct({})", func_name_, args_);
  }
  return fmt::format("{}({})", func_name_, args_);
}

auto BoundExpressionListRef::ToString() const -> std::string {
  return fmt::format("BoundExpressionListRef {{ identifier={}, values={} }}", identifier_, values_);
}

auto BoundSubqueryRef::ToString() const -> std::string {
  return fmt::format("BoundSubqueryRef {{\n  alias={},\n  subquery={},\n}}", alias_,
                     StringUtil::IndentAllLines(subquery_->ToString(), 2, true));
}

}  // namespace bustub
