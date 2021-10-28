//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregation_plan.h
//
// Identification: src/include/execution/plans/aggregation_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <utility>
#include <vector>

#include "common/util/hash_util.h"
#include "execution/plans/abstract_plan.h"
#include "storage/table/tuple.h"

namespace bustub {

/** AggregationType enumerates all the possible aggregation functions in our system */
enum class AggregationType { CountAggregate, SumAggregate, MinAggregate, MaxAggregate };

/**
 * AggregationPlanNode represents the various SQL aggregation functions.
 * For example, COUNT(), SUM(), MIN() and MAX().
 *
 * NOTE: To simplify this project, AggregationPlanNode must always have exactly one child.
 */
class AggregationPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new AggregationPlanNode.
   * @param output_schema The output format of this plan node
   * @param child The child plan to aggregate data over
   * @param having The having clause of the aggregation
   * @param group_bys The group by clause of the aggregation
   * @param aggregates The expressions that we are aggregating
   * @param agg_types The types that we are aggregating
   */
  AggregationPlanNode(const Schema *output_schema, const AbstractPlanNode *child, const AbstractExpression *having,
                      std::vector<const AbstractExpression *> &&group_bys,
                      std::vector<const AbstractExpression *> &&aggregates, std::vector<AggregationType> &&agg_types)
      : AbstractPlanNode(output_schema, {child}),
        having_(having),
        group_bys_(std::move(group_bys)),
        aggregates_(std::move(aggregates)),
        agg_types_(std::move(agg_types)) {}

  /** @return The type of the plan node */
  PlanType GetType() const override { return PlanType::Aggregation; }

  /** @return the child of this aggregation plan node */
  const AbstractPlanNode *GetChildPlan() const {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Aggregation expected to only have one child.");
    return GetChildAt(0);
  }

  /** @return The having clause */
  const AbstractExpression *GetHaving() const { return having_; }

  /** @return The idx'th group by expression */
  const AbstractExpression *GetGroupByAt(uint32_t idx) const { return group_bys_[idx]; }

  /** @return The group by expressions */
  const std::vector<const AbstractExpression *> &GetGroupBys() const { return group_bys_; }

  /** @return The idx'th aggregate expression */
  const AbstractExpression *GetAggregateAt(uint32_t idx) const { return aggregates_[idx]; }

  /** @return The aggregate expressions */
  const std::vector<const AbstractExpression *> &GetAggregates() const { return aggregates_; }

  /** @return The aggregate types */
  const std::vector<AggregationType> &GetAggregateTypes() const { return agg_types_; }

 private:
  /** A HAVING clause expression (may be `nullptr`) */
  const AbstractExpression *having_;
  /** The GROUP BY expressions */
  std::vector<const AbstractExpression *> group_bys_;
  /** The aggregation expressions */
  std::vector<const AbstractExpression *> aggregates_;
  /** The aggregation types */
  std::vector<AggregationType> agg_types_;
};

/** AggregateKey represents a key in an aggregation operation */
struct AggregateKey {
  /** The group-by values */
  std::vector<Value> group_bys_;

  /**
   * Compares two aggregate keys for equality.
   * @param other the other aggregate key to be compared with
   * @return `true` if both aggregate keys have equivalent group-by expressions, `false` otherwise
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

/** AggregateValue represents a value for each of the running aggregates */
struct AggregateValue {
  /** The aggregate values */
  std::vector<Value> aggregates_;
};

}  // namespace bustub

namespace std {

/** Implements std::hash on AggregateKey */
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
