//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_join_plan.h
//
// Identification: src/include/execution/plans/hash_join_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <utility>
#include <vector>

#include "execution/plans/abstract_plan.h"

namespace bustub {

/**
 * Hash join performs a JOIN operation with a hash table.
 */
class HashJoinPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new HashJoinPlanNode instance.
   * @param output_schema The output schema for the JOIN
   * @param children The child plans from which tuples are obtained
   * @param left_key_expression The expression for the left JOIN key
   * @param right_key_expression The expression for the right JOIN key
   */
  HashJoinPlanNode(const Schema *output_schema, std::vector<const AbstractPlanNode *> &&children,
                   const AbstractExpression *left_key_expression, const AbstractExpression *right_key_expression)
      : AbstractPlanNode(output_schema, std::move(children)),
        left_key_expression_{left_key_expression},
        right_key_expression_{right_key_expression} {}

  /** @return The type of the plan node */
  PlanType GetType() const override { return PlanType::HashJoin; }

  /** @return The expression to compute the left join key */
  const AbstractExpression *LeftJoinKeyExpression() const { return left_key_expression_; }

  /** @return The expression to compute the right join key */
  const AbstractExpression *RightJoinKeyExpression() const { return right_key_expression_; }

  /** @return The left plan node of the hash join */
  const AbstractPlanNode *GetLeftPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 2, "Hash joins should have exactly two children plans.");
    return GetChildAt(0);
  }

  /** @return The right plan node of the hash join */
  const AbstractPlanNode *GetRightPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 2, "Hash joins should have exactly two children plans.");
    return GetChildAt(1);
  }

 private:
  /** The expression to compute the left JOIN key */
  const AbstractExpression *left_key_expression_;
  /** The expression to compute the right JOIN key */
  const AbstractExpression *right_key_expression_;
};

}  // namespace bustub
