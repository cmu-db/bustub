//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// array_expression.h
//
// Identification: src/include/execution/expressions/array_expression.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "common/exception.h"
#include "execution/expressions/abstract_expression.h"
#include "fmt/ranges.h"
#include "type/value_factory.h"

namespace bustub {
/**
 * ArrayExpression represents arrays.
 */
class ArrayExpression : public AbstractExpression {
 public:
  /** Creates a new constant value expression wrapping the given value. */
  explicit ArrayExpression(const std::vector<AbstractExpressionRef> &children)
      : AbstractExpression(children, Column{"<val>", TypeId::VECTOR, static_cast<uint32_t>(children.size())}) {}

  auto Evaluate(const Tuple *tuple, const Schema &schema) const -> Value override {
    std::vector<double> values;
    values.reserve(children_.size());
    for (const auto &expr : children_) {
      auto val = expr->Evaluate(tuple, schema);
      if (val.GetTypeId() != TypeId::DECIMAL) {
        throw Exception("vector value can only be constructed from decimal type");
      }
      values.emplace_back(val.GetAs<double>());
    }
    return ValueFactory::GetVectorValue(values);
  }

  auto EvaluateJoin(const Tuple *left_tuple, const Schema &left_schema, const Tuple *right_tuple,
                    const Schema &right_schema) const -> Value override {
    std::vector<double> values;
    values.resize(children_.size());
    for (const auto &expr : children_) {
      auto val = expr->EvaluateJoin(left_tuple, left_schema, right_tuple, right_schema);
      if (val.GetTypeId() != TypeId::DECIMAL) {
        throw Exception("vector value can only be constructed from decimal type");
      }
      values.emplace_back(val.GetAs<double>());
    }
    return ValueFactory::GetVectorValue(values);
  }

  /** @return the string representation of the plan node and its children */
  auto ToString() const -> std::string override { return fmt::format("[{}]", fmt::join(children_, ",")); }

  BUSTUB_EXPR_CLONE_WITH_CHILDREN(ArrayExpression);
};
}  // namespace bustub
