#include "binder/bound_expression.h"
#include "binder/statement/select_statement.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/arithmetic_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/comparison_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/expressions/logic_expression.h"
#include "planner/planner.h"

namespace bustub {
// NOLINTNEXTLINE - weird error on clang-tidy.
auto Planner::GetAggCallFromFactory(const std::string &func_name, std::vector<AbstractExpressionRef> args)
    -> std::tuple<AggregationType, std::vector<AbstractExpressionRef>> {
  if (args.empty()) {
    if (func_name == "count_star") {
      return {AggregationType::CountStarAggregate, {}};
    }
  }
  if (args.size() == 1) {
    auto expr = std::move(args[0]);
    if (func_name == "min") {
      return {AggregationType::MinAggregate, {std::move(expr)}};
    }
    if (func_name == "max") {
      return {AggregationType::MaxAggregate, {std::move(expr)}};
    }
    if (func_name == "sum") {
      return {AggregationType::SumAggregate, {std::move(expr)}};
    }
    if (func_name == "count") {
      return {AggregationType::CountAggregate, {std::move(expr)}};
    }
  }
  throw Exception(fmt::format("unsupported agg_call {} with {} args", func_name, args.size()));
}

auto Planner::GetBinaryExpressionFromFactory(const std::string &op_name, AbstractExpressionRef left,
                                             AbstractExpressionRef right) -> AbstractExpressionRef {
  if (op_name == "=" || op_name == "==") {
    return std::make_shared<ComparisonExpression>(std::move(left), std::move(right), ComparisonType::Equal);
  }
  if (op_name == "!=" || op_name == "<>") {
    return std::make_shared<ComparisonExpression>(std::move(left), std::move(right), ComparisonType::NotEqual);
  }
  if (op_name == "<") {
    return std::make_shared<ComparisonExpression>(std::move(left), std::move(right), ComparisonType::LessThan);
  }
  if (op_name == "<=") {
    return std::make_shared<ComparisonExpression>(std::move(left), std::move(right), ComparisonType::LessThanOrEqual);
  }
  if (op_name == ">") {
    return std::make_shared<ComparisonExpression>(std::move(left), std::move(right), ComparisonType::GreaterThan);
  }
  if (op_name == ">=") {
    return std::make_shared<ComparisonExpression>(std::move(left), std::move(right),
                                                  ComparisonType::GreaterThanOrEqual);
  }
  if (op_name == "+") {
    return std::make_shared<ArithmeticExpression>(std::move(left), std::move(right), ArithmeticType::Plus);
  }
  if (op_name == "-") {
    return std::make_shared<ArithmeticExpression>(std::move(left), std::move(right), ArithmeticType::Minus);
  }
  if (op_name == "and") {
    return std::make_shared<LogicExpression>(std::move(left), std::move(right), LogicType::And);
  }
  if (op_name == "or") {
    return std::make_shared<LogicExpression>(std::move(left), std::move(right), LogicType::Or);
  }

  throw Exception(fmt::format("binary op {} not supported in planner yet", op_name));
}

}  // namespace bustub
