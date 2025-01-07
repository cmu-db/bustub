//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// string_expression.h
//
// Identification: src/include/execution/expressions/string_expression.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <algorithm>
#include <cctype>
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

enum class StringExpressionType { Lower, Upper };

/**
 * StringExpression represents two expressions being computed.
 */
class StringExpression : public AbstractExpression {
 public:
  StringExpression(AbstractExpressionRef arg, StringExpressionType expr_type)
      : AbstractExpression({std::move(arg)}, Column{"<val>", TypeId::VARCHAR, 256 /* hardcode max length */}),
        expr_type_{expr_type} {
    if (GetChildAt(0)->GetReturnType().GetType() != TypeId::VARCHAR) {
      BUSTUB_ENSURE(GetChildAt(0)->GetReturnType().GetType() == TypeId::VARCHAR, "unexpected arg");
    }
  }

  auto Compute(const std::string &val) const -> std::string {
    // TODO(student): implement upper / lower.
    return {};
  }

  auto Evaluate(const Tuple *tuple, const Schema &schema) const -> Value override {
    Value val = GetChildAt(0)->Evaluate(tuple, schema);
    auto str = val.GetAs<char *>();
    return ValueFactory::GetVarcharValue(Compute(str));
  }

  auto EvaluateJoin(const Tuple *left_tuple, const Schema &left_schema, const Tuple *right_tuple,
                    const Schema &right_schema) const -> Value override {
    Value val = GetChildAt(0)->EvaluateJoin(left_tuple, left_schema, right_tuple, right_schema);
    auto str = val.GetAs<char *>();
    return ValueFactory::GetVarcharValue(Compute(str));
  }

  /** @return the string representation of the expression node and its children */
  auto ToString() const -> std::string override { return fmt::format("{}({})", expr_type_, *GetChildAt(0)); }

  BUSTUB_EXPR_CLONE_WITH_CHILDREN(StringExpression);

  StringExpressionType expr_type_;

 private:
};
}  // namespace bustub

template <>
struct fmt::formatter<bustub::StringExpressionType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::StringExpressionType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::StringExpressionType::Upper:
        name = "upper";
        break;
      case bustub::StringExpressionType::Lower:
        name = "lower";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
