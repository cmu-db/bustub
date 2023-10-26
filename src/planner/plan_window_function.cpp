#include <memory>
#include <utility>

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/expressions/bound_alias.h"
#include "binder/expressions/bound_window.h"
#include "binder/statement/select_statement.h"
#include "binder/tokens.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/window_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"
#include "type/type_id.h"
#include "type/value_factory.h"

namespace bustub {

// TODO(chi): clang-tidy on macOS will suggest changing it to const reference. Looks like a bug.

void CheckOrderByCompatible(
    const std::vector<std::vector<std::pair<OrderByType, AbstractExpressionRef>>> &order_by_exprs) {
  if (order_by_exprs.empty()) {
    // either or window functions not having order by clause
    return;
  }
  // or all order by clause are the same
  std::vector<std::pair<OrderByType, AbstractExpressionRef>> first_order_by = order_by_exprs[0];
  for (auto &order_by : order_by_exprs) {
    if (order_by.size() != first_order_by.size()) {
      throw Exception("order by clause of window functions are not compatible");
    }
    for (uint32_t i = 0; i < order_by.size(); i++) {
      if (order_by[i].first != first_order_by[i].first) {
        throw Exception("order by clause of window functions are not compatible");
      }
      if (order_by[i].second->ToString() != first_order_by[i].second->ToString()) {
        throw Exception("order by clause of window functions are not compatible");
      }
    }
  }
}

/* NOLINTNEXTLINE */
auto Planner::PlanSelectWindow(const SelectStatement &statement, AbstractPlanNodeRef child) -> AbstractPlanNodeRef {
  /* For window function we don't do two passes rewrites like planning normal aggregations.
   *  Because standard sql does not allow using window function results in where clause, and
   *  our implementation does not support having on window function. We assume window functions
   *  only appear in select list, and we can plan them in one pass.
   */
  std::vector<AbstractExpressionRef> columns;
  std::vector<std::string> column_names;
  std::vector<uint32_t> window_func_indexes;
  std::vector<WindowFunctionType> window_func_types;
  std::vector<std::vector<AbstractExpressionRef>> partition_by_exprs;
  std::vector<std::vector<std::pair<OrderByType, AbstractExpressionRef>>> order_by_exprs;
  std::vector<AbstractExpressionRef> arg_exprs;

  for (uint32_t i = 0; i < statement.select_list_.size(); i++) {
    const auto &item = statement.select_list_[i];
    if (!item->HasWindowFunction()) {
      // normal select
      auto [name, expr] = PlanExpression(*item, {child});
      if (name == UNNAMED_COLUMN) {
        name = fmt::format("__unnamed#{}", universal_id_++);
      }
      columns.emplace_back(std::move(expr));
      column_names.emplace_back(std::move(name));
      continue;
    }

    // parse window function
    window_func_indexes.push_back(i);
    // we assign a -1 here as a placeholder
    columns.emplace_back(std::make_shared<ColumnValueExpression>(0, -1, TypeId::INTEGER));

    const BoundExpression *window_item = nullptr;
    if (item->type_ == ExpressionType::ALIAS) {
      const auto &alias_expr = dynamic_cast<const BoundAlias &>(*item);
      column_names.emplace_back(alias_expr.alias_);
      window_item = &(*alias_expr.child_);
    } else {
      BUSTUB_ASSERT(item->type_ == ExpressionType::WINDOW, "Invalid expression type has window function");
      column_names.emplace_back(fmt::format("__unnamed#{}", universal_id_++));
      window_item = &(*item);
    }
    const auto &window_call = dynamic_cast<const BoundWindow &>(*window_item);
    if (window_call.start_ != WindowBoundary::UNBOUNDED_PRECEDING ||
        (window_call.end_ != WindowBoundary::CURRENT_ROW_ROWS &&
         window_call.end_ != WindowBoundary::CURRENT_ROW_RANGE)) {
      throw Exception("Bustub currently only support window function with default window frame settings");
    }
    std::vector<AbstractExpressionRef> partition_by;
    for (auto &item : window_call.partition_by_) {
      auto [_, expr] = PlanExpression(*item, {child});
      partition_by.emplace_back(std::move(expr));
    }
    partition_by_exprs.emplace_back(std::move(partition_by));

    if (window_call.func_name_ == "rank" && window_call.order_bys_.empty()) {
      throw Exception("order by clause is mandatory for rank function");
    }

    std::vector<std::pair<OrderByType, AbstractExpressionRef>> order_by;
    for (const auto &item : window_call.order_bys_) {
      auto [_, expr] = PlanExpression(*item->expr_, {child});
      auto abstract_expr = std::move(expr);
      order_by.emplace_back(item->type_, abstract_expr);
    }
    order_by_exprs.emplace_back(std::move(order_by));

    std::vector<AbstractExpressionRef> raw_args;
    AbstractExpressionRef clean_arg;
    for (const auto &arg : window_call.args_) {
      auto [_, ret] = PlanExpression(*arg, {child});
      raw_args.emplace_back(std::move(ret));
    }
    auto [window_func_type, clean_args] = GetWindowAggCallFromFactory(window_call.func_name_, std::move(raw_args));
    window_func_types.emplace_back(window_func_type);
    if (clean_args.size() > 1) {
      throw bustub::NotImplementedException("only agg call of zero/one arg is supported");
    }
    if (clean_args.empty()) {
      // Rewrite count(*)/row_number into count(1)
      clean_arg = std::make_shared<ConstantValueExpression>(ValueFactory::GetIntegerValue(1));
    } else {
      clean_arg = std::move(clean_args[0]);
    }
    arg_exprs.emplace_back(std::move(clean_arg));
  }

  CheckOrderByCompatible(order_by_exprs);

  // we don't need window_agg_indexes here because we already use placeholders to infer the window agg column type is
  // Integer
  auto window_output_schema = WindowFunctionPlanNode::InferWindowSchema(columns);

  auto plan = std::make_shared<WindowFunctionPlanNode>(
      std::make_shared<Schema>(ProjectionPlanNode::RenameSchema(window_output_schema, column_names)), child,
      window_func_indexes, columns, partition_by_exprs, order_by_exprs, arg_exprs, window_func_types);

  return plan;
}

}  // namespace bustub
