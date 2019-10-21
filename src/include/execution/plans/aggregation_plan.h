//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregation_plan.h
//
// Identification: src/include/execution/plans/aggregation_plan.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <utility>
#include <vector>

#include "common/util/hash_util.h"
#include "execution/plans/abstract_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/** AggregationType enumerates all the possible aggregation functions in our system. */
enum class AggregationType { CountAggregate, SumAggregate, MinAggregate, MaxAggregate };

/**
 * AggregationPlanNode represents the various SQL aggregation functions.
 * For example, COUNT(), SUM(), MIN() and MAX().
 * To simplfiy this project, AggregationPlanNode must always have exactly one child.
 */
class AggregationPlanNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new AggregationPlanNode.
   * @param output_schema the output format of this plan node
   * @param child the child plan to aggregate data over
   * @param having the having clause of the aggregation
   * @param group_bys the group by clause of the aggregation
   * @param aggregates the expressions that we are aggregating
   * @param agg_types the types that we are aggregating
   */
  AggregationPlanNode(const Schema *output_schema, const AbstractPlanNode *child, const AbstractExpression *having,
                      std::vector<const AbstractExpression *> &&group_bys,
                      std::vector<const AbstractExpression *> &&aggregates, std::vector<AggregationType> &&agg_types)
      : AbstractPlanNode(output_schema, {child}),
        having_(having),
        group_bys_(std::move(group_bys)),
        aggregates_(std::move(aggregates)),
        agg_types_(std::move(agg_types)) {}

  PlanType GetType() const override { return PlanType::Aggregation; }

  /** @return the child of this aggregation plan node */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Aggregation expected to only have one child.");
    return GetChildAt(0);
  }

  /** @return the having clause */
  const AbstractExpression *GetHaving() const { return having_; }

  /** @return the idx'th group by expression */
  const AbstractExpression *GetGroupByAt(uint32_t idx) const { return group_bys_[idx]; }

  /** @return the group by expressions */
  const std::vector<const AbstractExpression *> &GetGroupBys() const { return group_bys_; }

  /** @return the idx'th aggregate expression */
  const AbstractExpression *GetAggregateAt(uint32_t idx) const { return aggregates_[idx]; }

  /** @return the aggregate expressions */
  const std::vector<const AbstractExpression *> &GetAggregates() const { return aggregates_; }

  /** @return the aggregate types */
  const std::vector<AggregationType> &GetAggregateTypes() const { return agg_types_; }

 private:
  const AbstractExpression *having_;
  std::vector<const AbstractExpression *> group_bys_;
  std::vector<const AbstractExpression *> aggregates_;
  std::vector<AggregationType> agg_types_;
};

struct AggregateKey {
  std::vector<Value> group_bys_;

  /**
   * Compares two aggregate keys for equality.
   * @param other the other aggregate key to be compared with
   * @return true if both aggregate keys have equivalent group-by expressions, false otherwise
   */
  bool operator==(const AggregateKey &other) const {
    for (uint32_t i = 0; i < other.group_bys_.size(); i++) {
      if (group_bys_[i].CompareEquals(other.group_bys_[i]) != CmpBool::CmpTrue) {
        return false;
      }
    }
    return true;
  }
};

struct AggregateValue {
  std::vector<Value> aggregates_;
};
}  // namespace bustub

namespace std {

/**
 * Implements std::hash on AggregateKey.
 */
template <>
struct hash<bustub::AggregateKey> {
  std::size_t operator()(const bustub::AggregateKey &agg_key) const {
    size_t curr_hash = 0;
    for (const auto &key : agg_key.group_bys_) {
      if (!key.IsNull()) {
        curr_hash = bustub::HashUtil::CombineHashes(curr_hash, bustub::HashUtil::HashValue(&key));
      }
    }
    return curr_hash;
  }
};

}  // namespace std
