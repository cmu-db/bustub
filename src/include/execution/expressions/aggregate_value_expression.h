//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregate_value_expression.h
//
// Identification: src/include/expression/aggregate_value_expression.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <vector>

#include "catalog/schema.h"
#include "execution/expressions/abstract_expression.h"
#include "storage/table/tuple.h"

namespace bustub {
/**
 * AggregateValueExpression represents aggregations such as MAX(a), MIN(b), COUNT(c)
 */
class AggregateValueExpression : public AbstractExpression {
 public:
  /**
   * Creates a new AggregateValueExpression.
   * @param is_group_by_term true if this is a group by
   * @param term_idx the index of the term
   * @param ret_type the return type of the aggregate value expression
   */
  AggregateValueExpression(bool is_group_by_term, uint32_t term_idx, TypeId ret_type)
      : AbstractExpression({}, ret_type), is_group_by_term_{is_group_by_term}, term_idx_{term_idx} {}

  /** Invalid operation for `AggregateValueExpression` */
  Value Evaluate(const Tuple *tuple, const Schema *schema) const override {
    UNREACHABLE("Aggregation should only refer to group-by and aggregates.");
  }

  /** Invalid operation for `AggregateValueExpression` */
  Value EvaluateJoin(const Tuple *left_tuple, const Schema *left_schema, const Tuple *right_tuple,
                     const Schema *right_schema) const override {
    UNREACHABLE("Aggregation should only refer to group-by and aggregates.");
  }

  /**
   * Returns the value obtained by evaluating the aggregates.
   * @param group_bys The group by values
   * @param aggregates The aggregate values
   * @return The value obtained by checking the aggregates and group-bys
   */
  Value EvaluateAggregate(const std::vector<Value> &group_bys, const std::vector<Value> &aggregates) const override {
    return is_group_by_term_ ? group_bys[term_idx_] : aggregates[term_idx_];
  }

 private:
  /** The flag indicating if this expression is a group-by term */
  bool is_group_by_term_;
  /** The index of the term in the collection of aggregates */
  uint32_t term_idx_;
};
}  // namespace bustub
