#include <memory>
#include <tuple>
#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_alias.h"
#include "binder/expressions/bound_binary_op.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/expressions/bound_func_call.h"
#include "binder/expressions/bound_unary_op.h"
#include "binder/statement/select_statement.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"

namespace bustub {

auto Planner::PlanBinaryOp(const BoundBinaryOp &expr, const std::vector<AbstractPlanNodeRef> &children)
    -> AbstractExpressionRef {
  auto [_1, left] = PlanExpression(*expr.larg_, children);
  auto [_2, right] = PlanExpression(*expr.rarg_, children);
  const auto &op_name = expr.op_name_;
  return GetBinaryExpressionFromFactory(op_name, std::move(left), std::move(right));
}

auto Planner::PlanColumnRef(const BoundColumnRef &expr, const std::vector<AbstractPlanNodeRef> &children)
    -> std::tuple<std::string, std::shared_ptr<ColumnValueExpression>> {
  if (children.empty()) {
    throw Exception("column ref should have at least one child");
  }

  auto col_name = expr.ToString();

  if (children.size() == 1) {
    // Projections, Filters, and other executors evaluating expressions with one single child will
    // use this branch.
    const auto &child = children[0];
    auto schema = child->OutputSchema();
    // Before we can call `schema.GetColIdx`,  we need to ensure there's no duplicated column.
    bool found = false;
    for (const auto &col : schema.GetColumns()) {
      if (col_name == col.GetName()) {
        if (found) {
          throw bustub::Exception("duplicated column found in schema");
        }
        found = true;
      }
    }
    uint32_t col_idx = schema.GetColIdx(col_name);
    auto col_type = schema.GetColumn(col_idx).GetType();
    return std::make_tuple(col_name, std::make_shared<ColumnValueExpression>(0, col_idx, col_type));
  }
  if (children.size() == 2) {
    /*
     * Joins will use this branch to plan expressions.
     *
     * If an expression is for join condition, e.g.
     * SELECT * from test_1 inner join test_2 on test_1.colA = test_2.col2
     * The plan will be like:
     * ```
     * NestedLoopJoin condition={ ColumnRef 0.0=ColumnRef 1.1 }
     *   SeqScan colA, colB
     *   SeqScan col1, col2
     * ```
     * In `ColumnRef n.m`, when executor is using the expression, it picks from its
     * nth children's mth column to get the data.
     */

    const auto &left = children[0];
    const auto &right = children[1];
    auto left_schema = left->OutputSchema();
    auto right_schema = right->OutputSchema();

    auto col_idx_left = left_schema.TryGetColIdx(col_name);
    auto col_idx_right = right_schema.TryGetColIdx(col_name);
    if (col_idx_left && col_idx_right) {
      throw bustub::Exception(fmt::format("ambiguous column name {}", col_name));
    }
    if (col_idx_left) {
      auto col_type = left_schema.GetColumn(*col_idx_left).GetType();
      return std::make_tuple(col_name, std::make_shared<ColumnValueExpression>(0, *col_idx_left, col_type));
    }
    if (col_idx_right) {
      auto col_type = right_schema.GetColumn(*col_idx_right).GetType();
      return std::make_tuple(col_name, std::make_shared<ColumnValueExpression>(1, *col_idx_right, col_type));
    }
    throw bustub::Exception(fmt::format("column name {} not found", col_name));
  }
  UNREACHABLE("no executor with expression has more than 2 children for now");
}

auto Planner::PlanConstant(const BoundConstant &expr, const std::vector<AbstractPlanNodeRef> &children)
    -> AbstractExpressionRef {
  return std::make_shared<ConstantValueExpression>(expr.val_);
}

void Planner::AddAggCallToContext(BoundExpression &expr) {
  switch (expr.type_) {
    case ExpressionType::AGG_CALL: {
      auto &agg_call_expr = dynamic_cast<BoundAggCall &>(expr);
      auto agg_name = fmt::format("__pseudo_agg#{}", ctx_.aggregations_.size());
      auto agg_call =
          BoundAggCall(agg_name, agg_call_expr.is_distinct_, std::vector<std::unique_ptr<BoundExpression>>{});
      // Replace the agg call in the original bound expression with a pseudo one, add agg call to the context.
      ctx_.AddAggregation(std::make_unique<BoundAggCall>(std::exchange(agg_call_expr, std::move(agg_call))));
      return;
    }
    case ExpressionType::COLUMN_REF: {
      return;
    }
    case ExpressionType::BINARY_OP: {
      auto &binary_op_expr = dynamic_cast<BoundBinaryOp &>(expr);
      AddAggCallToContext(*binary_op_expr.larg_);
      AddAggCallToContext(*binary_op_expr.rarg_);
      return;
    }
    case ExpressionType::FUNC_CALL: {
      auto &func_call_expr = dynamic_cast<BoundFuncCall &>(expr);
      for (const auto &child : func_call_expr.args_) {
        AddAggCallToContext(*child);
      }
      return;
    }
    case ExpressionType::CONSTANT: {
      return;
    }
    case ExpressionType::ALIAS: {
      auto &alias_expr = dynamic_cast<const BoundAlias &>(expr);
      AddAggCallToContext(*alias_expr.child_);
      return;
    }
    default:
      break;
  }
  throw Exception(fmt::format("expression type {} not supported in planner yet", expr.type_));
}

auto Planner::PlanExpression(const BoundExpression &expr, const std::vector<AbstractPlanNodeRef> &children)
    -> std::tuple<std::string, AbstractExpressionRef> {
  switch (expr.type_) {
    case ExpressionType::AGG_CALL: {
      if (ctx_.next_aggregation_ >= ctx_.expr_in_agg_.size()) {
        throw bustub::Exception("unexpected agg call");
      }
      return std::make_tuple(UNNAMED_COLUMN, std::move(ctx_.expr_in_agg_[ctx_.next_aggregation_++]));
    }
    case ExpressionType::COLUMN_REF: {
      const auto &column_ref_expr = dynamic_cast<const BoundColumnRef &>(expr);
      return PlanColumnRef(column_ref_expr, children);
    }
    case ExpressionType::BINARY_OP: {
      const auto &binary_op_expr = dynamic_cast<const BoundBinaryOp &>(expr);
      return std::make_tuple(UNNAMED_COLUMN, PlanBinaryOp(binary_op_expr, children));
    }
    case ExpressionType::FUNC_CALL: {
      const auto &func_call_expr = dynamic_cast<const BoundFuncCall &>(expr);
      return std::make_tuple(UNNAMED_COLUMN, PlanFuncCall(func_call_expr, children));
    }
    case ExpressionType::CONSTANT: {
      const auto &constant_expr = dynamic_cast<const BoundConstant &>(expr);
      return std::make_tuple(UNNAMED_COLUMN, PlanConstant(constant_expr, children));
    }
    case ExpressionType::ALIAS: {
      const auto &alias_expr = dynamic_cast<const BoundAlias &>(expr);
      auto [_1, expr] = PlanExpression(*alias_expr.child_, children);
      return std::make_tuple(alias_expr.alias_, std::move(expr));
    }
    default:
      break;
  }
  throw Exception(fmt::format("expression type {} not supported in planner yet", expr.type_));
}

}  // namespace bustub
