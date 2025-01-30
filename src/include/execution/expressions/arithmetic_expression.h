//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// arithmetic_expression.h
//
// Identification: src/include/execution/expressions/arithmetic_expression.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "catalog/schema.h"
#include "common/exception.h"
#include "common/macros.h"
#include "execution/expressions/abstract_expression.h"
#include "fmt/format.h"
#include "storage/table/tuple.h"
#include "type/type.h"
#include "type/type_id.h"
#include "type/value_factory.h"

namespace bustub {

/** ArithmeticType represents the type of computation that we want to perform. */
enum class ArithmeticType { Plus, Minus };

/**
 * ArithmeticExpression represents two expressions being computed, ONLY SUPPORT INTEGER FOR NOW.
 */
class ArithmeticExpression : public AbstractExpression {
 public:
  /** Creates a new comparison expression representing (left comp_type right). */
  ArithmeticExpression(AbstractExpressionRef left, AbstractExpressionRef right, ArithmeticType compute_type)
      : AbstractExpression({std::move(left), std::move(right)}, Column{"<val>", TypeId::INTEGER}),
        compute_type_{compute_type} {
    if (GetChildAt(0)->GetReturnType().GetType() != TypeId::INTEGER ||
        GetChildAt(1)->GetReturnType().GetType() != TypeId::INTEGER) {
      throw bustub::NotImplementedException("only support integer for now");
    }
  }

  auto Evaluate(const Tuple *tuple, const Schema &schema) const -> Value override {
    Value lhs = GetChildAt(0)->Evaluate(tuple, schema);
    Value rhs = GetChildAt(1)->Evaluate(tuple, schema);
    auto res = PerformComputation(lhs, rhs);
    if (res == std::nullopt) {
      return ValueFactory::GetNullValueByType(TypeId::INTEGER);
    }
    return ValueFactory::GetIntegerValue(*res);
  }

  auto EvaluateJoin(const Tuple *left_tuple, const Schema &left_schema, const Tuple *right_tuple,
                    const Schema &right_schema) const -> Value override {
    Value lhs = GetChildAt(0)->EvaluateJoin(left_tuple, left_schema, right_tuple, right_schema);
    Value rhs = GetChildAt(1)->EvaluateJoin(left_tuple, left_schema, right_tuple, right_schema);
    auto res = PerformComputation(lhs, rhs);
    if (res == std::nullopt) {
      return ValueFactory::GetNullValueByType(TypeId::INTEGER);
    }
    return ValueFactory::GetIntegerValue(*res);
  }

  /** @return the string representation of the expression node and its children */
  auto ToString() const -> std::string override {
    return fmt::format("({}{}{})", *GetChildAt(0), compute_type_, *GetChildAt(1));
  }

  BUSTUB_EXPR_CLONE_WITH_CHILDREN(ArithmeticExpression);

  ArithmeticType compute_type_;

 private:
  auto PerformComputation(const Value &lhs, const Value &rhs) const -> std::optional<int32_t> {
    if (lhs.IsNull() || rhs.IsNull()) {
      return std::nullopt;
    }
    switch (compute_type_) {
      case ArithmeticType::Plus:
        return lhs.GetAs<int32_t>() + rhs.GetAs<int32_t>();
      case ArithmeticType::Minus:
        return lhs.GetAs<int32_t>() - rhs.GetAs<int32_t>();
      default:
        UNREACHABLE("Unsupported arithmetic type.");
    }
  }
};
}  // namespace bustub

template <>
struct fmt::formatter<bustub::ArithmeticType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::ArithmeticType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::ArithmeticType::Plus:
        name = "+";
        break;
      case bustub::ArithmeticType::Minus:
        name = "-";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
