#include <algorithm>
#include <memory>

#include "binder/bound_expression.h"
#include "binder/statement/insert_statement.h"
#include "common/exception.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/insert_plan.h"
#include "planner/planner.h"

namespace bustub {

auto Planner::PlanInsert(const InsertStatement &statement) -> AbstractPlanNodeRef {
  auto select = PlanSelect(*statement.select_);

  auto table = catalog_.GetTable(statement.table_);
  BUSTUB_ASSERT(table, "table not found");

  const auto &table_schema = table->schema_.GetColumns();
  const auto &child_schema = select->OutputSchema().GetColumns();
  if (!std::equal(table_schema.cbegin(), table_schema.cend(), child_schema.cbegin(), child_schema.cend(),
                  [](auto &&col1, auto &&col2) { return col1.GetType() == col2.GetType(); })) {
    throw bustub::Exception("table schema mismatch");
  }

  return std::make_shared<InsertPlanNode>(std::move(select), table->oid_);
}

}  // namespace bustub
