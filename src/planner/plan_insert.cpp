#include <algorithm>
#include <memory>
#include <unordered_map>

#include "binder/bound_expression.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/statement/update_statement.h"
#include "binder/tokens.h"
#include "catalog/column.h"
#include "catalog/schema.h"
#include "common/exception.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/delete_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/insert_plan.h"
#include "execution/plans/update_plan.h"
#include "execution/plans/values_plan.h"
#include "planner/planner.h"
#include "type/type_id.h"

namespace bustub {

auto Planner::PlanInsert(const InsertStatement &statement) -> AbstractPlanNodeRef {
  auto select = PlanSelect(*statement.select_);

  const auto &table_schema = statement.table_->schema_.GetColumns();
  const auto &child_schema = select->OutputSchema().GetColumns();
  if (!std::equal(table_schema.cbegin(), table_schema.cend(), child_schema.cbegin(), child_schema.cend(),
                  [](auto &&col1, auto &&col2) { return col1.GetType() == col2.GetType(); })) {
    throw bustub::Exception("table schema mismatch");
  }

  auto insert_schema = std::make_shared<Schema>(std::vector{Column("__bustub_internal.insert_rows", TypeId::INTEGER)});

  return std::make_shared<InsertPlanNode>(std::move(insert_schema), std::move(select), statement.table_->oid_);
}

auto Planner::PlanDelete(const DeleteStatement &statement) -> AbstractPlanNodeRef {
  auto table = PlanTableRef(*statement.table_);
  auto [_, condition] = PlanExpression(*statement.expr_, {table});
  auto filter = std::make_shared<FilterPlanNode>(table->output_schema_, std::move(condition), std::move(table));
  auto delete_schema = std::make_shared<Schema>(std::vector{Column("__bustub_internal.delete_rows", TypeId::INTEGER)});

  return std::make_shared<DeletePlanNode>(std::move(delete_schema), std::move(filter), statement.table_->oid_);
}

auto Planner::PlanUpdate(const UpdateStatement &statement) -> AbstractPlanNodeRef {
  auto table = PlanTableRef(*statement.table_);
  auto [_, condition] = PlanExpression(*statement.filter_expr_, {table});
  AbstractPlanNodeRef filter =
      std::make_shared<FilterPlanNode>(table->output_schema_, std::move(condition), std::move(table));

  auto scope = std::vector{filter};

  std::vector<AbstractExpressionRef> target_exprs;
  target_exprs.resize(filter->output_schema_->GetColumnCount());

  for (const auto &[col, target_expr] : statement.target_expr_) {
    auto [_1, target_abstract_expr] = PlanExpression(*target_expr, scope);
    auto [_2, col_abstract_expr] = PlanColumnRef(*col, scope);
    target_exprs[col_abstract_expr->GetColIdx()] = std::move(target_abstract_expr);
  }

  for (size_t idx = 0; idx < target_exprs.size(); idx++) {
    if (target_exprs[idx] == nullptr) {
      target_exprs[idx] =
          std::make_shared<ColumnValueExpression>(0, idx, filter->output_schema_->GetColumn(idx).GetType());
    }
  }

  auto update_schema = std::make_shared<Schema>(std::vector{Column("__bustub_internal.update_rows", TypeId::INTEGER)});

  return std::make_shared<UpdatePlanNode>(std::move(update_schema), std::move(filter), statement.table_->oid_,
                                          std::move(target_exprs));
}

}  // namespace bustub
