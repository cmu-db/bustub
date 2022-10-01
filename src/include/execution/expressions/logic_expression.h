//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// logic_expression.h
//
// Identification: src/include/expression/logic_expression.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

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

/** ArithmeticType represents the type of logic operation that we want to perform. */
enum class LogicType { And, Or };

/**
 * LogicExpression represents two expressions being computed.
 */
class LogicExpression : public AbstractExpression {
 public:
  /** Creates a new comparison expression representing (left comp_type right). */
  LogicExpression(AbstractExpressionRef left, AbstractExpressionRef right, LogicType logic_type)
      : AbstractExpression({std::move(left), std::move(right)}, TypeId::BOOLEAN), logic_type_{logic_type} {
    if (GetChildAt(0)->GetReturnType() != TypeId::BOOLEAN || GetChildAt(1)->GetReturnType() != TypeId::BOOLEAN) {
      throw bustub::NotImplementedException("expect boolean from either side");
    }
  }

  auto Evaluate(const Tuple *tuple, const Schema &schema) const -> Value override {
    Value lhs = GetChildAt(0)->Evaluate(tuple, schema);
    Value rhs = GetChildAt(1)->Evaluate(tuple, schema);
    return ValueFactory::GetBooleanValue(PerformComputation(lhs, rhs));
  }

  auto EvaluateJoin(const Tuple *left_tuple, const Schema &left_schema, const Tuple *right_tuple,
                    const Schema &right_schema) const -> Value override {
    Value lhs = GetChildAt(0)->EvaluateJoin(left_tuple, left_schema, right_tuple, right_schema);
    Value rhs = GetChildAt(1)->EvaluateJoin(left_tuple, left_schema, right_tuple, right_schema);
    return ValueFactory::GetBooleanValue(PerformComputation(lhs, rhs));
  }

  /** @return the string representation of the expression node and its children */
  auto ToString() const -> std::string override {
    return fmt::format("({}{}{})", *GetChildAt(0), logic_type_, *GetChildAt(1));
  }

  BUSTUB_EXPR_CLONE_WITH_CHILDREN(LogicExpression);

  LogicType logic_type_;

 private:
  auto GetBoolAsCmpBool(const Value &val) const -> CmpBool {
    if (val.IsNull()) {
      return CmpBool::CmpNull;
    }
    if (val.GetAs<bool>()) {
      return CmpBool::CmpTrue;
    }
    return CmpBool::CmpFalse;
  }

  auto PerformComputation(const Value &lhs, const Value &rhs) const -> CmpBool {
    auto l = GetBoolAsCmpBool(lhs);
    auto r = GetBoolAsCmpBool(rhs);
    switch (logic_type_) {
      case LogicType::And:
        if (l == CmpBool::CmpFalse || r == CmpBool::CmpFalse) {
          return CmpBool::CmpFalse;
        }
        if (l == CmpBool::CmpTrue && r == CmpBool::CmpTrue) {
          return CmpBool::CmpTrue;
        }
        return CmpBool::CmpNull;
      case LogicType::Or:
        if (l == CmpBool::CmpFalse && r == CmpBool::CmpFalse) {
          return CmpBool::CmpFalse;
        }
        if (l == CmpBool::CmpTrue || r == CmpBool::CmpTrue) {
          return CmpBool::CmpTrue;
        }
        return CmpBool::CmpNull;
      default:
        UNREACHABLE("Unsupported logic type.");
    }
  }
};
}  // namespace bustub

template <>
struct fmt::formatter<bustub::LogicType> : formatter<string_view> {
  template <typename FormatContext>
  auto format(bustub::LogicType c, FormatContext &ctx) const {
    string_view name;
    switch (c) {
      case bustub::LogicType::And:
        name = "and";
        break;
      case bustub::LogicType::Or:
        name = "or";
        break;
      default:
        name = "Unknown";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
