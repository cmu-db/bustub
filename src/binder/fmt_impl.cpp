#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_constant.h"
#include "binder/expressions/bound_func_call.h"
#include "binder/expressions/bound_window.h"
#include "binder/statement/select_statement.h"
#include "binder/table_ref/bound_cte_ref.h"
#include "binder/table_ref/bound_expression_list_ref.h"
#include "binder/table_ref/bound_subquery_ref.h"
#include "common/util/string_util.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace bustub {

auto BoundFuncCall::ToString() const -> std::string { return fmt::format("{}({})", func_name_, args_); }

auto BoundAggCall::ToString() const -> std::string {
  if (is_distinct_) {
    return fmt::format("{}_distinct({})", func_name_, args_);
  }
  return fmt::format("{}({})", func_name_, args_);
}

auto BoundExpressionListRef::ToString() const -> std::string {
  return fmt::format("BoundExpressionListRef {{ identifier={}, values={} }}", identifier_, values_);
}

auto BoundCTERef::ToString() const -> std::string {
  return fmt::format("BoundCTERef {{ alias={}, cte={} }}", alias_, cte_name_);
}

auto BoundSubqueryRef::ToString() const -> std::string {
  std::vector<std::string> columns;
  for (const auto &name : select_list_name_) {
    columns.push_back(fmt::format("{}", fmt::join(name, ".")));
  }
  return fmt::format("BoundSubqueryRef {{\n  alias={},\n  subquery={},\n  columns={},\n}}", alias_,
                     StringUtil::IndentAllLines(subquery_->ToString(), 2, true), columns);
}

auto BoundWindow::ToString() const -> std::string {
  std::vector<std::string> partition_by;
  for (const auto &expr : partition_by_) {
    partition_by.push_back(expr->ToString());
  }

  std::vector<std::string> order_bys;
  for (const auto &expr : order_bys_) {
    order_bys.push_back(expr->ToString());
  }

  std::string start_offset = "None";
  if (start_offset_.has_value()) {
    start_offset = (*start_offset_)->ToString();
  }

  std::string end_offset = "None";
  if (end_offset_.has_value()) {
    end_offset = (*end_offset_)->ToString();
  }

  std::string start_mode = Binder::WindowBoundaryToString(start_);
  std::string end_mode = Binder::WindowBoundaryToString(end_);

  // TODO(avery): add frame
  return fmt::format("{}({}) Over {{  partition_by={},  order_by={} }}", func_name_, args_,
                     StringUtil::IndentAllLines(fmt::format("[{}]", fmt::join(partition_by, ", ")), 2, true),
                     StringUtil::IndentAllLines(fmt::format("[{}]", fmt::join(order_bys, ", ")), 2, true));
}

}  // namespace bustub
