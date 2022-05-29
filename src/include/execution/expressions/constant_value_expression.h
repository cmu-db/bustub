//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// constant_value_expression.h
//
// Identification: src/include/expression/constant_value_expression.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <vector>

#include "execution/expressions/abstract_expression.h"

namespace bustub {
/**
 * ConstantValueExpression represents constants.
 */
class ConstantValueExpression : public AbstractExpression {
 public:
  /** Creates a new constant value expression wrapping the given value. */
  explicit ConstantValueExpression(const Value &val) : AbstractExpression({}, val.GetTypeId()), val_(val) {}

  auto Evaluate(const Tuple *tuple, const Schema *schema) const -> Value override { return val_; }

  auto EvaluateJoin(const Tuple *left_tuple, const Schema *left_schema, const Tuple *right_tuple,
                    const Schema *right_schema) const -> Value override {
    return val_;
  }

  auto EvaluateAggregate(const std::vector<Value> &group_bys, const std::vector<Value> &aggregates) const
      -> Value override {
    return val_;
  }

 private:
  Value val_;
};
}  // namespace bustub
