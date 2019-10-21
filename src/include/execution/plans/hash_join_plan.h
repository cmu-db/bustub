//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_join_plan.h
//
// Identification: src/include/execution/plans/hash_join_plan.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <utility>
#include <vector>

#include "execution/plans/abstract_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * HashJoinPlanNode is used to represent performing a hash join between two children plan nodes.
 * By convention, the left child (index 0) is used to build the hash table,
 * and the right child (index 1) is used in probing the hash table.
 */
class HashJoinPlanNode : public AbstractPlanNode {
 public:
  HashJoinPlanNode(const Schema *output_schema, std::vector<const AbstractPlanNode *> &&children,
                   const AbstractExpression *predicate, std::vector<const AbstractExpression *> &&left_hash_keys,
                   std::vector<const AbstractExpression *> &&right_hash_keys)
      : AbstractPlanNode(output_schema, std::move(children)),
        predicate_(predicate),
        left_hash_keys_(std::move(left_hash_keys)),
        right_hash_keys_(std::move(right_hash_keys)) {}

  PlanType GetType() const override { return PlanType::HashJoin; }

  /** @return the predicate to be used in the hash join */
  const AbstractExpression *Predicate() const { return predicate_; }

  /** @return the left plan node of the hash join, by convention this is used to build the table */
  const AbstractPlanNode *GetLeftPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 2, "Hash joins should have exactly two children plans.");
    return GetChildAt(0);
  }

  /** @return the right plan node of the hash join */
  const AbstractPlanNode *GetRightPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 2, "Hash joins should have exactly two children plans.");
    return GetChildAt(1);
  }

  /** @return the left key at the given index */
  const AbstractExpression *GetLeftKeyAt(uint32_t idx) const { return left_hash_keys_[idx]; }

  /** @return the left keys */
  const std::vector<const AbstractExpression *> &GetLeftKeys() const { return left_hash_keys_; }

  /** @return the right key at the given index */
  const AbstractExpression *GetRightKeyAt(uint32_t idx) const { return right_hash_keys_[idx]; }

  /** @return the right keys */
  const std::vector<const AbstractExpression *> &GetRightKeys() const { return right_hash_keys_; }

 private:
  /** The hash join predicate. */
  const AbstractExpression *predicate_;
  /** The left child's hash keys. */
  std::vector<const AbstractExpression *> left_hash_keys_;
  /** The right child's hash keys. */
  std::vector<const AbstractExpression *> right_hash_keys_;
};
}  // namespace bustub
