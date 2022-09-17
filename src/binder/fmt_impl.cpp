#include "binder/expressions/bound_agg_call.h"
#include "binder/table_ref/bound_expression_list_ref.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace bustub {

auto BoundAggCall::ToString() const -> std::string { return fmt::format("{}({})", func_name_, args_); }

auto BoundExpressionListRef::ToString() const -> std::string {
  return fmt::format("BoundExpressionListRef {{ values={} }}", values_);
}

}  // namespace bustub
