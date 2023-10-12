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

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "binder/bound_order_by.h"
#include "common/util/hash_util.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "fmt/format.h"
#include "storage/table/tuple.h"

namespace bustub {

/** WindowAggregationType enumerates all the possible window aggregation functions in our system */
enum class WindowAggregationType { CountStarAggregate, CountAggregate, SumAggregate, MinAggregate, MaxAggregate };

class WindowAggregationPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new WindowAggregationPlanNode.
   * @param output_schema The output format of this plan node
   * @param child The child plan to aggregate data over
   * @param partition_by The partition by clause of the window aggregation
   * @param order_by The order by clause of the window aggregation
   * @param aggregates The expressions that we are aggregating
   * @param window_agg_types The types that we are aggregating
   */
  WindowAggregationPlanNode(SchemaRef output_schema, AbstractPlanNodeRef child, int window_agg_id,
                            std::vector<AbstractExpressionRef> columns, std::vector<AbstractExpressionRef> partition_by,
                            std::vector<std::pair<OrderByType, AbstractExpressionRef>> order_by,
                            AbstractExpressionRef aggregate, WindowAggregationType window_agg_type)
      : AbstractPlanNode(std::move(output_schema), {std::move(child)}),
        window_agg_id_(window_agg_id),
        columns_(std::move(columns)),
        partition_by_(std::move(partition_by)),
        order_by_(std::move(order_by)),
        aggregate_(std::move(aggregate)),
        window_agg_type_(window_agg_type) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Window; }

  /** @return the child of this aggregation plan node */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Window Aggregation expected to only have one child.");
    return GetChildAt(0);
  }

  /** @return The idx'th partition by expression */
  auto GetPartitionByAt(uint32_t idx) const -> const AbstractExpressionRef & { return partition_by_[idx]; }

  /** @return The partition by expressions */
  auto GetPartitionBys() const -> const std::vector<AbstractExpressionRef> & { return partition_by_; }

  /** @return The idx'th order by expression */
  auto GetOrderByAt(uint32_t idx) const -> const std::pair<OrderByType, AbstractExpressionRef> & {
    return order_by_[idx];
  }

  /** @return The order by expressions */
  auto GetOrderBys() const -> const std::vector<std::pair<OrderByType, AbstractExpressionRef>> & { return order_by_; }

  /** @return The aggregate expression */
  auto GetAggregate() const -> const AbstractExpressionRef & { return aggregate_; }

  /** @return The window aggregate type */
  auto GetWindowAggregateType() const -> const WindowAggregationType & { return window_agg_type_; }

  static auto InferWindowSchema(const std::vector<AbstractExpressionRef> &columns, int window_agg_id) -> Schema;

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(WindowAggregationPlanNode);

  /** The Window Aggregation column index */
  int window_agg_id_;

  /** all other columns expressions */
  std::vector<AbstractExpressionRef> columns_;

  /** The PARTITION BY expressions */
  std::vector<AbstractExpressionRef> partition_by_;
  /** The ORDER BY expressions */
  std::vector<std::pair<OrderByType, AbstractExpressionRef>> order_by_;

  /** The aggregation expressions */
  AbstractExpressionRef aggregate_;
  /** The aggregation type */
  WindowAggregationType window_agg_type_;

 protected:
  auto PlanNodeToString() const -> std::string override;
};

}  // namespace bustub

template <>
struct fmt::formatter<bustub::WindowAggregationType> : formatter<std::string> {
  template <typename FormatContext>
  auto format(bustub::WindowAggregationType c, FormatContext &ctx) const {
    using bustub::WindowAggregationType;
    std::string name = "unknown";
    switch (c) {
      case WindowAggregationType::CountStarAggregate:
        name = "count_star";
        break;
      case WindowAggregationType::CountAggregate:
        name = "count";
        break;
      case WindowAggregationType::SumAggregate:
        name = "sum";
        break;
      case WindowAggregationType::MinAggregate:
        name = "min";
        break;
      case WindowAggregationType::MaxAggregate:
        name = "max";
        break;
    }
    return formatter<std::string>::format(name, ctx);
  }
};
