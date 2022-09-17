#include <memory>
#include <utility>

#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "binder/table_ref/bound_cross_product_ref.h"
#include "binder/table_ref/bound_expression_list_ref.h"
#include "binder/table_ref/bound_join_ref.h"
#include "catalog/column.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/mock_scan_plan.h"
#include "execution/plans/nested_loop_join_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "execution/plans/values_plan.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "planner/planner.h"
#include "type/value_factory.h"

namespace bustub {

auto Planner::PlanTableRef(const BoundTableRef &table_ref) -> std::unique_ptr<AbstractPlanNode> {
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
    default:
      break;
  }
  throw Exception(fmt::format("the table ref type {} is not supported in planner yet", table_ref.type_));
}

auto Planner::PlanBaseTableRef(const BoundBaseTableRef &table_ref) -> std::unique_ptr<AbstractPlanNode> {
  // We always scan ALL columns of the table, and use projection executor to
  // remove some of them, therefore simplifying the planning process.

  // It is also possible to have a "virtual" logical projection node, and
  // we can merge it with table scan when optimizing. But for now, having
  // an optimizer or not remains undecided. So I'd prefer going with a new
  // ProjectionExecutor.

  auto table = catalog_.GetTable(table_ref.table_);
  BUSTUB_ASSERT(table, "table not found");
  const auto &schema = table->schema_;
  std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;

  size_t idx = 0;
  for (const auto &column : schema.GetColumns()) {
    output_schema.emplace_back(fmt::format("{}.{}", table_ref.table_, column.GetName()),
                               SaveExpression(std::make_unique<ColumnValueExpression>(0, idx, column.GetType())));
    idx += 1;
  }

  if (StringUtil::StartsWith(table->name_, "__")) {
    // Plan as MockScanExecutor if it is a mock table.
    if (StringUtil::StartsWith(table->name_, "__mock")) {
      return std::make_unique<MockScanPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), 100, table->name_);
    }
    throw bustub::Exception(fmt::format("unsupported internal table: {}", table->name_));
  }
  // Otherwise, plan as normal SeqScan.
  return std::make_unique<SeqScanPlanNode>(SaveSchema(MakeOutputSchema(output_schema)), nullptr, table->oid_);
}

auto Planner::PlanCrossProductRef(const BoundCrossProductRef &table_ref) -> std::unique_ptr<AbstractPlanNode> {
  auto left = PlanTableRef(*table_ref.left_);
  auto right = PlanTableRef(*table_ref.right_);
  std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;
  size_t idx = 0;
  for (const auto &column : left->OutputSchema()->GetColumns()) {
    output_schema.emplace_back(column.GetName(),
                               SaveExpression(std::make_unique<ColumnValueExpression>(0, idx, column.GetType())));
    idx += 1;
  }
  idx = 0;
  for (const auto &column : right->OutputSchema()->GetColumns()) {
    output_schema.emplace_back(column.GetName(),
                               SaveExpression(std::make_unique<ColumnValueExpression>(1, idx, column.GetType())));
    idx += 1;
  }
  return std::make_unique<NestedLoopJoinPlanNode>(
      SaveSchema(MakeOutputSchema(output_schema)),
      std::vector{SavePlanNode(std::move(left)), SavePlanNode(std::move(right))},
      SaveExpression(std::make_unique<ConstantValueExpression>(ValueFactory::GetBooleanValue(true))));
}

auto Planner::PlanJoinRef(const BoundJoinRef &table_ref) -> std::unique_ptr<AbstractPlanNode> {
  auto left = PlanTableRef(*table_ref.left_);
  auto right = PlanTableRef(*table_ref.right_);
  std::vector<std::pair<std::string, const AbstractExpression *>> output_schema;
  size_t idx = 0;
  for (const auto &column : left->OutputSchema()->GetColumns()) {
    output_schema.emplace_back(column.GetName(),
                               SaveExpression(std::make_unique<ColumnValueExpression>(0, idx, column.GetType())));
    idx += 1;
  }
  idx = 0;
  for (const auto &column : right->OutputSchema()->GetColumns()) {
    output_schema.emplace_back(column.GetName(),
                               SaveExpression(std::make_unique<ColumnValueExpression>(1, idx, column.GetType())));
    idx += 1;
  }
  auto nlj_output_schema = SaveSchema(MakeOutputSchema(output_schema));
  auto [_, join_condition] = PlanExpression(*table_ref.condition_, {left.get(), right.get()});
  auto nlj_node = std::make_unique<NestedLoopJoinPlanNode>(
      nlj_output_schema, std::vector{SavePlanNode(std::move(left)), SavePlanNode(std::move(right))},
      SaveExpression(std::move(join_condition)));
  return nlj_node;
}

auto Planner::PlanExpressionListRef(const BoundExpressionListRef &table_ref) -> std::unique_ptr<AbstractPlanNode> {
  std::vector<std::vector<const AbstractExpression *>> all_exprs;
  for (const auto &row : table_ref.values_) {
    std::vector<const AbstractExpression *> row_exprs;
    for (const auto &col : row) {
      auto [_, expr] = PlanExpression(*col, {});
      row_exprs.push_back(SaveExpression(std::move(expr)));
    }
    all_exprs.emplace_back(std::move(row_exprs));
  }

  const auto &first_row = all_exprs[0];
  std::vector<Column> cols;
  cols.reserve(first_row.size());
  size_t idx = 0;
  for (const auto &col : first_row) {
    auto col_name = fmt::format("__values.{}", idx);
    if (col->GetReturnType() != TypeId::VARCHAR) {
      cols.emplace_back(std::move(col_name), col->GetReturnType(), nullptr);
    } else {
      cols.emplace_back(std::move(col_name), col->GetReturnType(), VARCHAR_DEFAULT_LENGTH, nullptr);
    }
    idx += 1;
  }
  auto schema = std::make_unique<Schema>(cols);

  return std::make_unique<ValuesPlanNode>(SaveSchema(std::move(schema)), std::move(all_exprs));
}

}  // namespace bustub
