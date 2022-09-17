#include <memory>

#include "binder/bound_expression.h"
#include "binder/statement/insert_statement.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/insert_plan.h"
#include "planner/planner.h"

namespace bustub {

auto Planner::PlanInsert(const InsertStatement &statement) -> std::unique_ptr<AbstractPlanNode> {
  auto select = PlanSelect(*statement.select_);

  auto table = catalog_.GetTable(statement.table_);
  BUSTUB_ASSERT(table, "table not found");

  return std::make_unique<InsertPlanNode>(SavePlanNode(std::move(select)), table->oid_);
}

}  // namespace bustub
