#include <memory>
#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_binary_op.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/expressions/bound_unary_op.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/comparison_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "fmt/format.h"
#include "planner/planner.h"

namespace bustub {

const char *unnamed_column = "<unnamed>";

auto Planner::PlanBinaryOp(const BoundBinaryOp &expr, const std::vector<const AbstractPlanNode *> &children)
    -> std::unique_ptr<AbstractExpression> {
  auto [_1, left] = PlanExpression(*expr.larg_, children);
  auto [_2, right] = PlanExpression(*expr.rarg_, children);
  const auto &op_name = expr.op_name_;

  if (op_name == "=" || op_name == "==") {
    return std::make_unique<ComparisonExpression>(SaveExpression(std::move(left)), SaveExpression(std::move(right)),
                                                  ComparisonType::Equal);
  }
  if (op_name == "!=" || op_name == "<>") {
    return std::make_unique<ComparisonExpression>(SaveExpression(std::move(left)), SaveExpression(std::move(right)),
                                                  ComparisonType::NotEqual);
  }
  if (op_name == "<") {
    return std::make_unique<ComparisonExpression>(SaveExpression(std::move(left)), SaveExpression(std::move(right)),
                                                  ComparisonType::LessThan);
  }
  if (op_name == "<=") {
    return std::make_unique<ComparisonExpression>(SaveExpression(std::move(left)), SaveExpression(std::move(right)),
                                                  ComparisonType::LessThanOrEqual);
  }
  if (op_name == ">") {
    return std::make_unique<ComparisonExpression>(SaveExpression(std::move(left)), SaveExpression(std::move(right)),
                                                  ComparisonType::GreaterThan);
  }
  if (op_name == ">=") {
    return std::make_unique<ComparisonExpression>(SaveExpression(std::move(left)), SaveExpression(std::move(right)),
                                                  ComparisonType::GreaterThanOrEqual);
  }

  throw Exception(fmt::format("binary op {} not supported in planner yet", op_name));
}

auto Planner::PlanColumnRef(const BoundColumnRef &expr, const std::vector<const AbstractPlanNode *> &children)
    -> std::tuple<std::string, std::unique_ptr<AbstractExpression>> {
  if (children.empty()) {
    throw Exception("column ref should have at least one child");
  }

  auto col_name = fmt::format("{}.{}", expr.table_, expr.col_);

  if (children.size() == 1) {
    // Projections, Filters, and other executors evaluating expressions with one single child will
    // use this branch.
    const auto &child = children[0];
    auto schema = child->OutputSchema();
    uint32_t col_idx = schema->GetColIdx(col_name);
    auto col_type = schema->GetColumn(col_idx).GetType();
    return std::make_tuple(col_name, std::make_unique<ColumnValueExpression>(0, col_idx, col_type));
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

    auto col_idx_left = left_schema->TryGetColIdx(col_name);
    auto col_idx_right = right_schema->TryGetColIdx(col_name);
    if (col_idx_left && col_idx_right) {
      throw bustub::Exception(fmt::format("ambiguous column name {}", col_name));
    }
    if (col_idx_left) {
      auto col_type = left_schema->GetColumn(*col_idx_left).GetType();
      return std::make_tuple(col_name, std::make_unique<ColumnValueExpression>(0, *col_idx_left, col_type));
    }
    if (col_idx_right) {
      auto col_type = right_schema->GetColumn(*col_idx_right).GetType();
      return std::make_tuple(col_name, std::make_unique<ColumnValueExpression>(1, *col_idx_right, col_type));
    }
    throw bustub::Exception(fmt::format("column name {} not found", col_name));
  }
  UNREACHABLE("no executor with expression has more than 2 children for now");
}

auto Planner::PlanConstant(const BoundConstant &expr, const std::vector<const AbstractPlanNode *> &children)
    -> std::unique_ptr<AbstractExpression> {
  return std::make_unique<ConstantValueExpression>(expr.val_);
}

auto Planner::PlanExpression(const BoundExpression &expr, const std::vector<const AbstractPlanNode *> &children)
    -> std::tuple<std::string, std::unique_ptr<AbstractExpression>> {
  switch (expr.type_) {
    case ExpressionType::AGG_CALL:
      throw Exception("agg call should be handled by PlanSelect");
    case ExpressionType::COLUMN_REF: {
      const auto &column_ref_expr = dynamic_cast<const BoundColumnRef &>(expr);
      return PlanColumnRef(column_ref_expr, children);
    }
    case ExpressionType::BINARY_OP: {
      const auto &binary_op_expr = dynamic_cast<const BoundBinaryOp &>(expr);
      return std::make_tuple(unnamed_column, PlanBinaryOp(binary_op_expr, children));
    }
    case ExpressionType::CONSTANT: {
      const auto &constant_expr = dynamic_cast<const BoundConstant &>(expr);
      return std::make_tuple(unnamed_column, PlanConstant(constant_expr, children));
    }
    default:
      break;
  }
  throw Exception(fmt::format("expression type {} not supported in planner yet", expr.type_));
}

}  // namespace bustub
