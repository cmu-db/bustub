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
   * @param window_agg_indexes The indexes of the window aggregation functions
   * @param columns All columns include the placeholder for window aggregation functions
   * @param partition_bys The partition by clause of the window aggregations
   * @param order_bys The order by clause of the window aggregations
   * @param aggregates The expressions that we are aggregating
   * @param window_agg_types The types that we are aggregating
   *
   * Window Aggregation is different from normal aggregation as it outputs one row for each inputing rows,
   * and can be combined with normal selected columns. The columns in WindowAggregationPlanNode contains both
   * normal selected columns and placeholder columns for window aggregations.
   *
   * For example, if we have a query like:
   *    SELECT 0.1, 0.2, SUM(0.3) OVER (PARTITION BY 0.2 ORDER BY 0.3), SUM(0.4) OVER (PARTITION BY 0.1 ORDER BY
   * 0.2,0.3) FROM table;
   *
   * The WindowAggregationPlanNode should contains following structure:
   *    columns: std::vector<AbstractExpressionRef>{0.1, 0.2, 0.-1(placeholder), 0.-1(placeholder)}
   *    partition_bys: std::vector<std::vector<AbstractExpressionRef>>{{0.2}, {0.1}}
   *    order_bys: std::vector<std::vector<AbstractExpressionRef>>{{0.3}, {0.2,0.3}}
   *    aggregates: std::vector<AbstractExpressionRef>{0.3, 0.4}
   *    window_agg_types: std::vector<WindowAggregationType>{SumAggregate, SumAggregate}
   */
  WindowAggregationPlanNode(SchemaRef output_schema, AbstractPlanNodeRef child,
                            std::vector<uint32_t> window_agg_indexes, std::vector<AbstractExpressionRef> columns,
                            std::vector<std::vector<AbstractExpressionRef>> partition_bys,
                            std::vector<std::vector<std::pair<OrderByType, AbstractExpressionRef>>> order_bys,
                            std::vector<AbstractExpressionRef> aggregates,
                            std::vector<WindowAggregationType> window_agg_types)
      : AbstractPlanNode(std::move(output_schema), {std::move(child)}),
        window_agg_indexes_(std::move(window_agg_indexes)),
        columns_(std::move(columns)),
        partition_bys_(std::move(partition_bys)),
        order_bys_(std::move(order_bys)),
        aggregates_(std::move(aggregates)),
        window_agg_types_(std::move(window_agg_types)) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::Window; }

  /** @return the child of this aggregation plan node */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "Window Aggregation expected to only have one child.");
    return GetChildAt(0);
  }

  /** @return The partition by expressions */
  auto GetPartitionBys() const -> const std::vector<std::vector<AbstractExpressionRef>> & { return partition_bys_; }

  /** @return The order by expressions */
  auto GetOrderBys() const -> const std::vector<std::vector<std::pair<OrderByType, AbstractExpressionRef>>> & {
    return order_bys_;
  }

  /** @return The aggregate expression */
  auto GetAggregates() const -> const std::vector<AbstractExpressionRef> & { return aggregates_; }

  /** @return The window aggregate type */
  auto GetWindowAggregateTypes() const -> const std::vector<WindowAggregationType> & { return window_agg_types_; }

  static auto InferWindowSchema(const std::vector<AbstractExpressionRef> &columns) -> Schema;

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(WindowAggregationPlanNode);

  /** The Window Aggregation column index */
  std::vector<uint32_t> window_agg_indexes_;

  /** all columns expressions */
  std::vector<AbstractExpressionRef> columns_;

  /** The PARTITION BY expressions */
  std::vector<std::vector<AbstractExpressionRef>> partition_bys_;
  /** The ORDER BY expressions */
  std::vector<std::vector<std::pair<OrderByType, AbstractExpressionRef>>> order_bys_;

  /** The aggregation expressions */
  std::vector<AbstractExpressionRef> aggregates_;
  /** The aggregation types */
  std::vector<WindowAggregationType> window_agg_types_;

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
