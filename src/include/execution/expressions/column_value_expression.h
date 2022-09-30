//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// column_value_expression.h
//
// Identification: src/include/expression/column_value_expression.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "catalog/schema.h"
#include "execution/expressions/abstract_expression.h"
#include "storage/table/tuple.h"

namespace bustub {
/**
 * ColumnValueExpression maintains the tuple index and column index relative to a particular schema or join.
 */
class ColumnValueExpression : public AbstractExpression {
 public:
  /**
   * ColumnValueExpression is an abstraction around "Table.member" in terms of indexes.
   * @param tuple_idx {tuple index 0 = left side of join, tuple index 1 = right side of join}
   * @param col_idx the index of the column in the schema
   * @param ret_type the return type of the expression
   */
  ColumnValueExpression(uint32_t tuple_idx, uint32_t col_idx, TypeId ret_type)
      : AbstractExpression({}, ret_type), tuple_idx_{tuple_idx}, col_idx_{col_idx} {}

  auto Evaluate(const Tuple *tuple, const Schema &schema) const -> Value override {
    return tuple->GetValue(&schema, col_idx_);
  }

  auto EvaluateJoin(const Tuple *left_tuple, const Schema &left_schema, const Tuple *right_tuple,
                    const Schema &right_schema) const -> Value override {
    return tuple_idx_ == 0 ? left_tuple->GetValue(&left_schema, col_idx_)
                           : right_tuple->GetValue(&right_schema, col_idx_);
  }

  auto GetTupleIdx() const -> uint32_t { return tuple_idx_; }
  auto GetColIdx() const -> uint32_t { return col_idx_; }

  /** @return the string representation of the plan node and its children */
  auto ToString() const -> std::string override { return fmt::format("#{}.{}", tuple_idx_, col_idx_); }

  BUSTUB_EXPR_CLONE_WITH_CHILDREN(ColumnValueExpression);

 private:
  /** Tuple index 0 = left side of join, tuple index 1 = right side of join */
  uint32_t tuple_idx_;
  /** Column index refers to the index within the schema of the tuple, e.g. schema {A,B,C} has indexes {0,1,2} */
  uint32_t col_idx_;
};
}  // namespace bustub
