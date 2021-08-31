//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// aggregation_executor.h
//
// Identification: src/include/execution/executors/aggregation_executor.h
//
// Copyright (c) 2015-19, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/util/hash_util.h"
#include "container/hash/hash_function.h"
#include "execution/executor_context.h"
#include "execution/executors/abstract_executor.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/aggregation_plan.h"
#include "storage/table/tuple.h"
#include "type/value_factory.h"

namespace bustub {
/**
 * A simplified hash table that has all the necessary functionality for aggregations.
 */
class SimpleAggregationHashTable {
 public:
  /**
   * Create a new simplified aggregation hash table.
   * @param agg_exprs the aggregation expressions
   * @param agg_types the types of aggregations
   */
  SimpleAggregationHashTable(const std::vector<const AbstractExpression *> &agg_exprs,
                             const std::vector<AggregationType> &agg_types)
      : agg_exprs_{agg_exprs}, agg_types_{agg_types} {}

  /** @return the initial aggregrate value for this aggregation executor */
  AggregateValue GenerateInitialAggregateValue() {
    std::vector<Value> values;
    for (const auto &agg_type : agg_types_) {
      switch (agg_type) {
        case AggregationType::CountAggregate:
          // Count starts at zero.
          values.emplace_back(ValueFactory::GetIntegerValue(0));
          break;
        case AggregationType::SumAggregate:
          // Sum starts at zero.
          values.emplace_back(ValueFactory::GetIntegerValue(0));
          break;
        case AggregationType::MinAggregate:
          // Min starts at INT_MAX.
          values.emplace_back(ValueFactory::GetIntegerValue(BUSTUB_INT32_MAX));
          break;
        case AggregationType::MaxAggregate:
          // Max starts at INT_MIN.
          values.emplace_back(ValueFactory::GetIntegerValue(BUSTUB_INT32_MIN));
          break;
      }
    }
    return {values};
  }

  /** Combines the input into the aggregation result. */
  void CombineAggregateValues(AggregateValue *result, const AggregateValue &input) {
    for (uint32_t i = 0; i < agg_exprs_.size(); i++) {
      switch (agg_types_[i]) {
        case AggregationType::CountAggregate:
          // Count increases by one.
          result->aggregates_[i] = result->aggregates_[i].Add(ValueFactory::GetIntegerValue(1));
          break;
        case AggregationType::SumAggregate:
          // Sum increases by addition.
          result->aggregates_[i] = result->aggregates_[i].Add(input.aggregates_[i]);
          break;
        case AggregationType::MinAggregate:
          // Min is just the min.
          result->aggregates_[i] = result->aggregates_[i].Min(input.aggregates_[i]);
          break;
        case AggregationType::MaxAggregate:
          // Max is just the max.
          result->aggregates_[i] = result->aggregates_[i].Max(input.aggregates_[i]);
          break;
      }
    }
  }

  /**
   * Inserts a value into the hash table and then combines it with the current aggregation.
   * @param agg_key the key to be inserted
   * @param agg_val the value to be inserted
   */
  void InsertCombine(const AggregateKey &agg_key, const AggregateValue &agg_val) {
    if (ht_.count(agg_key) == 0) {
      ht_.insert({agg_key, GenerateInitialAggregateValue()});
    }
    CombineAggregateValues(&ht_[agg_key], agg_val);
  }

  /**
   * An iterator through the simplified aggregation hash table.
   */
  class Iterator {
   public:
    /** Creates an iterator for the aggregate map. */
    explicit Iterator(std::unordered_map<AggregateKey, AggregateValue>::const_iterator iter) : iter_(iter) {}

    /** @return the key of the iterator */
    const AggregateKey &Key() { return iter_->first; }

    /** @return the value of the iterator */
    const AggregateValue &Val() { return iter_->second; }

    /** @return the iterator before it is incremented */
    Iterator &operator++() {
      ++iter_;
      return *this;
    }

    /** @return true if both iterators are identical */
    bool operator==(const Iterator &other) { return this->iter_ == other.iter_; }

    /** @return true if both iterators are different */
    bool operator!=(const Iterator &other) { return this->iter_ != other.iter_; }

   private:
    /** Aggregates map. */
    std::unordered_map<AggregateKey, AggregateValue>::const_iterator iter_;
  };

  /** @return iterator to the start of the hash table */
  Iterator Begin() { return Iterator{ht_.cbegin()}; }

  /** @return iterator to the end of the hash table */
  Iterator End() { return Iterator{ht_.cend()}; }

 private:
  /** The hash table is just a map from aggregate keys to aggregate values. */
  std::unordered_map<AggregateKey, AggregateValue> ht_{};
  /** The aggregate expressions that we have. */
  const std::vector<const AbstractExpression *> &agg_exprs_;
  /** The types of aggregations that we have. */
  const std::vector<AggregationType> &agg_types_;
};

/**
 * AggregationExecutor executes an aggregation operation (e.g. COUNT, SUM, MIN, MAX) on the tuples of a child executor.
 */
class AggregationExecutor : public AbstractExecutor {
 public:
  /**
   * Creates a new aggregation executor.
   * @param exec_ctx the context that the aggregation should be performed in
   * @param plan the aggregation plan node
   * @param child the child executor
   */
  AggregationExecutor(ExecutorContext *exec_ctx, const AggregationPlanNode *plan,
                      std::unique_ptr<AbstractExecutor> &&child);

  /** Do not use or remove this function, otherwise you will get zero points. */
  const AbstractExecutor *GetChildExecutor() const;

  const Schema *GetOutputSchema() override { return plan_->OutputSchema(); };

  void Init() override;

  bool Next(Tuple *tuple, RID *rid) override;

  /** @return the tuple as an AggregateKey */
  AggregateKey MakeKey(const Tuple *tuple) {
    std::vector<Value> keys;
    for (const auto &expr : plan_->GetGroupBys()) {
      keys.emplace_back(expr->Evaluate(tuple, child_->GetOutputSchema()));
    }
    return {keys};
  }

  /** @return the tuple as an AggregateValue */
  AggregateValue MakeVal(const Tuple *tuple) {
    std::vector<Value> vals;
    for (const auto &expr : plan_->GetAggregates()) {
      vals.emplace_back(expr->Evaluate(tuple, child_->GetOutputSchema()));
    }
    return {vals};
  }

 private:
  /** The aggregation plan node. */
  const AggregationPlanNode *plan_;
  /** The child executor whose tuples we are aggregating. */
  std::unique_ptr<AbstractExecutor> child_;
  /** Simple aggregation hash table. */
  // Uncomment me! SimpleAggregationHashTable aht_;
  /** Simple aggregation hash table iterator. */
  // Uncomment me! SimpleAggregationHashTable::Iterator aht_iterator_;
};
}  // namespace bustub
