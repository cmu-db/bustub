//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// comparison_expression.h
//
// Identification: src/include/execution/expressions/comparison_expression.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "catalog/schema.h"
#include "execution/expressions/abstract_expression.h"
#include "fmt/format.h"
#include "storage/table/tuple.h"
#include "type/value_factory.h"

namespace bustub {

/** ComparisonType represents the type of comparison that we want to perform. */
enum class ComparisonType { Equal, NotEqual, LessThan, LessThanOrEqual, GreaterThan, GreaterThanOrEqual };

/**
 * ComparisonExpression represents two expressions being compared.
 */
class ComparisonExpression : public AbstractExpression {
 public:
  /** Creates a new comparison expression representing (left comp_type right). */
  ComparisonExpression(AbstractExpressionRef left, AbstractExpressionRef right, ComparisonType comp_type)
      : AbstractExpression({std::move(left), std::move(right)}, Column{"<val>", TypeId::BOOLEAN}),
        comp_type_{comp_type} {}

  auto Evaluate(const Tuple *tuple, const Schema &schema) const -> Value override {
    Value lhs = GetChildAt(0)->Evaluate(tuple, schema);
    Value rhs = GetChildAt(1)->Evaluate(tuple, schema);
    return ValueFactory::GetBooleanValue(PerformComparison(lhs, rhs));
  }

  auto EvaluateJoin(const Tuple *left_tuple, const Schema &left_schema, const Tuple *right_tuple,
                    const Schema &right_schema) const -> Value override {
    Value lhs = GetChildAt(0)->EvaluateJoin(left_tuple, left_schema, right_tuple, right_schema);
    Value rhs = GetChildAt(1)->EvaluateJoin(left_tuple, left_schema, right_tuple, right_schema);
    return ValueFactory::GetBooleanValue(PerformComparison(lhs, rhs));
  }

  /** @return the string representation of the expression node and its children */
  auto ToString() const -> std::string override {
    return fmt::format("({}{}{})", *GetChildAt(0), comp_type_, *GetChildAt(1));
  }

  BUSTUB_EXPR_CLONE_WITH_CHILDREN(ComparisonExpression);

  ComparisonType comp_type_;

 private:
  auto PerformComparison(const Value &lhs, const Value &rhs) const -> CmpBool {
    switch (comp_type_) {
      case ComparisonType::Equal:
        return lhs.CompareEquals(rhs);
      case ComparisonType::NotEqual:
        return lhs.CompareNotEquals(rhs);
      case ComparisonType::LessThan:
        return lhs.CompareLessThan(rhs);
      case ComparisonType::LessThanOrEqual:
        return lhs.CompareLessThanEquals(rhs);
      case ComparisonType::GreaterThan:
        return lhs.CompareGreaterThan(rhs);
      case ComparisonType::GreaterThanOrEqual:
        return lhs.CompareGreaterThanEquals(rhs);
      default:
        BUSTUB_ASSERT(false, "Unsupported comparison type.");
    }
  }
};
}  // namespace bustub

template <>
struct fmt::formatter<bustub::ComparisonType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::ComparisonType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::ComparisonType::Equal:
        name = "=";
        break;
      case bustub::ComparisonType::NotEqual:
        name = "!=";
        break;
      case bustub::ComparisonType::LessThan:
        name = "<";
        break;
      case bustub::ComparisonType::LessThanOrEqual:
        name = "<=";
        break;
      case bustub::ComparisonType::GreaterThan:
        name = ">";
        break;
      case bustub::ComparisonType::GreaterThanOrEqual:
        name = ">=";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
