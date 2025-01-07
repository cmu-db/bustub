//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// plan_node.cpp
//
// Identification: src/execution/plan_node.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <memory>
#include <vector>
#include "binder/table_ref/bound_base_table_ref.h"
#include "catalog/catalog.h"
#include "catalog/column.h"
#include "catalog/schema.h"
#include "common/exception.h"
#include "execution/plans/aggregation_plan.h"
#include "execution/plans/nested_loop_join_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "execution/plans/window_plan.h"

namespace bustub {

auto SeqScanPlanNode::InferScanSchema(const BoundBaseTableRef &table) -> Schema {
  std::vector<Column> schema;
  for (const auto &column : table.schema_.GetColumns()) {
    auto col_name = fmt::format("{}.{}", table.GetBoundTableName(), column.GetName());
    schema.emplace_back(col_name, column);
  }
  return Schema(schema);
}

auto NestedLoopJoinPlanNode::InferJoinSchema(const AbstractPlanNode &left, const AbstractPlanNode &right) -> Schema {
  std::vector<Column> schema;
  for (const auto &column : left.OutputSchema().GetColumns()) {
    schema.emplace_back(column);
  }
  for (const auto &column : right.OutputSchema().GetColumns()) {
    schema.emplace_back(column);
  }
  return Schema(schema);
}

auto ProjectionPlanNode::InferProjectionSchema(const std::vector<AbstractExpressionRef> &expressions) -> Schema {
  std::vector<Column> schema;
  for (const auto &expr : expressions) {
    auto type_id = expr->GetReturnType();
    schema.emplace_back(expr->GetReturnType().WithColumnName("<unnamed>"));
  }
  return Schema(schema);
}

auto ProjectionPlanNode::RenameSchema(const Schema &schema, const std::vector<std::string> &col_names) -> Schema {
  std::vector<Column> output;
  if (col_names.size() != schema.GetColumnCount()) {
    throw bustub::Exception("mismatched number of columns");
  }
  size_t idx = 0;
  for (const auto &column : schema.GetColumns()) {
    output.emplace_back(col_names[idx++], column);
  }
  return Schema(output);
}

auto AggregationPlanNode::InferAggSchema(const std::vector<AbstractExpressionRef> &group_bys,
                                         const std::vector<AbstractExpressionRef> &aggregates,
                                         const std::vector<AggregationType> &agg_types) -> Schema {
  std::vector<Column> output;
  output.reserve(group_bys.size() + aggregates.size());
  for (const auto &column : group_bys) {
    output.emplace_back(column->GetReturnType().WithColumnName("<unnamed>"));
  }
  for (size_t idx = 0; idx < aggregates.size(); idx++) {
    // TODO(chi): correctly infer agg call return type
    output.emplace_back("<unnamed>", TypeId::INTEGER);
  }
  return Schema(output);
}

auto WindowFunctionPlanNode::InferWindowSchema(const std::vector<AbstractExpressionRef> &columns) -> Schema {
  std::vector<Column> output;
  output.reserve(columns.size());
  // TODO(avery): correctly infer window call return type
  for (const auto &column : columns) {
    output.emplace_back(column->GetReturnType().WithColumnName("<unnamed>"));
  }
  return Schema(output);
}

}  // namespace bustub
