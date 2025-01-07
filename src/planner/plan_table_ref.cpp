//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// plan_table_ref.cpp
//
// Identification: src/planner/plan_table_ref.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>
#include <utility>

#include "binder/bound_order_by.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/statement/select_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "binder/table_ref/bound_cross_product_ref.h"
#include "binder/table_ref/bound_cte_ref.h"
#include "binder/table_ref/bound_expression_list_ref.h"
#include "binder/table_ref/bound_join_ref.h"
#include "binder/table_ref/bound_subquery_ref.h"
#include "catalog/column.h"
#include "catalog/schema.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/mock_scan_plan.h"
#include "execution/plans/nested_loop_join_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "execution/plans/values_plan.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "planner/planner.h"
#include "type/value_factory.h"

namespace bustub {

/**
 * @brief Plan a `BoundTableRef`
 *
 * - For a BaseTableRef, this function will return a `SeqScanPlanNode`. Note that all tables with
 *   names beginning with `__` will be planned as `MockScanPlanNode`.
 * - For a `JoinRef` or `CrossProductRef`, this function will return a `NestedLoopJoinNode`.
 * @param table_ref the bound table ref from binder.
 * @return the plan node of this bound table ref.
 */
auto Planner::PlanTableRef(const BoundTableRef &table_ref) -> AbstractPlanNodeRef {
  switch (table_ref.type_) {
    case TableReferenceType::BASE_TABLE: {
      const auto &base_table_ref = dynamic_cast<const BoundBaseTableRef &>(table_ref);
      return PlanBaseTableRef(base_table_ref);
    }
    case TableReferenceType::CROSS_PRODUCT: {
      const auto &cross_product = dynamic_cast<const BoundCrossProductRef &>(table_ref);
      return PlanCrossProductRef(cross_product);
    }
    case TableReferenceType::JOIN: {
      const auto &join = dynamic_cast<const BoundJoinRef &>(table_ref);
      return PlanJoinRef(join);
    }
    case TableReferenceType::EXPRESSION_LIST: {
      const auto &expression_list = dynamic_cast<const BoundExpressionListRef &>(table_ref);
      return PlanExpressionListRef(expression_list);
    }
    case TableReferenceType::SUBQUERY: {
      const auto &subquery = dynamic_cast<const BoundSubqueryRef &>(table_ref);
      return PlanSubquery(subquery, subquery.alias_);
    }
    case TableReferenceType::CTE: {
      const auto &cte = dynamic_cast<const BoundCTERef &>(table_ref);
      return PlanCTERef(cte);
    }
    default:
      break;
  }
  throw Exception(fmt::format("the table ref type {} is not supported in planner yet", table_ref.type_));
}

auto Planner::PlanSubquery(const BoundSubqueryRef &table_ref, const std::string &alias) -> AbstractPlanNodeRef {
  auto select_node = PlanSelect(*table_ref.subquery_);
  std::vector<std::string> output_column_names;
  std::vector<AbstractExpressionRef> exprs;
  size_t idx = 0;

  // This projection will be removed by eliminate projection rule. It's solely used for renaming columns.
  for (const auto &col : select_node->OutputSchema().GetColumns()) {
    auto expr = std::make_shared<ColumnValueExpression>(0, idx, col);
    output_column_names.emplace_back(fmt::format("{}.{}", alias, fmt::join(table_ref.select_list_name_[idx], ".")));
    exprs.push_back(std::move(expr));
    idx++;
  }

  auto saved_child = std::move(select_node);

  return std::make_shared<ProjectionPlanNode>(
      std::make_shared<Schema>(
          ProjectionPlanNode::RenameSchema(ProjectionPlanNode::InferProjectionSchema(exprs), output_column_names)),
      std::move(exprs), saved_child);
}

auto Planner::PlanBaseTableRef(const BoundBaseTableRef &table_ref) -> AbstractPlanNodeRef {
  // We always scan ALL columns of the table, and use projection executor to
  // remove some of them, therefore simplifying the planning process.

  // It is also possible to have a "virtual" logical projection node, and
  // we can merge it with table scan when optimizing. But for now, having
  // an optimizer or not remains undecided. So I'd prefer going with a new
  // ProjectionExecutor.

  auto table = catalog_.GetTable(table_ref.table_);
  BUSTUB_ASSERT(table, "table not found");

  if (StringUtil::StartsWith(table->name_, "__")) {
    // Plan as MockScanExecutor if it is a mock table.
    if (StringUtil::StartsWith(table->name_, "__mock")) {
      return std::make_shared<MockScanPlanNode>(std::make_shared<Schema>(SeqScanPlanNode::InferScanSchema(table_ref)),
                                                table->name_);
    }
    throw bustub::Exception(fmt::format("unsupported internal table: {}", table->name_));
  }
  // Otherwise, plan as normal SeqScan.
  return std::make_shared<SeqScanPlanNode>(std::make_shared<Schema>(SeqScanPlanNode::InferScanSchema(table_ref)),
                                           table->oid_, table->name_);
}

auto Planner::PlanCrossProductRef(const BoundCrossProductRef &table_ref) -> AbstractPlanNodeRef {
  auto left = PlanTableRef(*table_ref.left_);
  auto right = PlanTableRef(*table_ref.right_);
  return std::make_shared<NestedLoopJoinPlanNode>(
      std::make_shared<Schema>(NestedLoopJoinPlanNode::InferJoinSchema(*left, *right)), std::move(left),
      std::move(right), std::make_shared<ConstantValueExpression>(ValueFactory::GetBooleanValue(true)),
      JoinType::INNER);
}

auto Planner::PlanCTERef(const BoundCTERef &table_ref) -> AbstractPlanNodeRef {
  for (const auto &cte : *ctx_.cte_list_) {
    if (cte->alias_ == table_ref.cte_name_) {
      return PlanSubquery(*cte, table_ref.alias_);
    }
  }
  UNREACHABLE("CTE not found");
}

auto Planner::PlanJoinRef(const BoundJoinRef &table_ref) -> AbstractPlanNodeRef {
  auto left = PlanTableRef(*table_ref.left_);
  auto right = PlanTableRef(*table_ref.right_);
  auto [_, join_condition] = PlanExpression(*table_ref.condition_, {left, right});
  auto nlj_node = std::make_shared<NestedLoopJoinPlanNode>(
      std::make_shared<Schema>(NestedLoopJoinPlanNode::InferJoinSchema(*left, *right)), std::move(left),
      std::move(right), std::move(join_condition), table_ref.join_type_);
  return nlj_node;
}

auto Planner::PlanExpressionListRef(const BoundExpressionListRef &table_ref) -> AbstractPlanNodeRef {
  std::vector<std::vector<AbstractExpressionRef>> all_exprs;
  for (const auto &row : table_ref.values_) {
    std::vector<AbstractExpressionRef> row_exprs;
    for (const auto &col : row) {
      auto [_, expr] = PlanExpression(*col, {});
      row_exprs.push_back(std::move(expr));
    }
    all_exprs.emplace_back(std::move(row_exprs));
  }

  const auto &first_row = all_exprs[0];
  std::vector<Column> cols;
  cols.reserve(first_row.size());
  size_t idx = 0;
  for (const auto &col : first_row) {
    auto col_name = fmt::format("{}.{}", table_ref.identifier_, idx);
    cols.emplace_back(col->GetReturnType().WithColumnName(col_name));
    idx += 1;
  }
  auto schema = std::make_shared<Schema>(cols);

  return std::make_shared<ValuesPlanNode>(std::move(schema), std::move(all_exprs));
}

}  // namespace bustub
