#include <algorithm>
#include <memory>
#include "catalog/column.h"
#include "catalog/schema.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/projection_plan.h"
#include "optimizer/optimizer.h"

namespace bustub {

auto Optimizer::OptimizeMergeProjection(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  std::vector<AbstractPlanNodeRef> children;
  for (const auto &child : plan->GetChildren()) {
    children.emplace_back(OptimizeMergeProjection(child));
  }
  if (plan->GetType() == PlanType::Projection) {
    const auto &projection_plan = dynamic_cast<const ProjectionPlanNode &>(*plan);
    // Has exactly one child
    BUSTUB_ENSURE(children.size() == 1, "Projection with multiple children?? That's weird!");
    // If the schema is the same (except column name)
    const auto &child_plan = children[0];
    const auto &child_schema = child_plan->OutputSchema();
    const auto &projection_schema = projection_plan.OutputSchema();
    const auto &child_columns = child_schema.GetColumns();
    const auto &projection_columns = projection_schema.GetColumns();
    if (std::equal(child_columns.begin(), child_columns.end(), projection_columns.begin(), projection_columns.end(),
                   [](auto &&child_col, auto &&proj_col) {
                     // TODO(chi): consider VARCHAR length
                     return child_col.GetType() == proj_col.GetType();
                   })) {
      const auto &exprs = projection_plan.GetExpressions();
      // If all items are column value expressions
      bool is_identical = true;
      for (size_t idx = 0; idx < exprs.size(); idx++) {
        auto column_value_expr = dynamic_cast<const ColumnValueExpression *>(exprs[idx].get());
        if (column_value_expr != nullptr) {
          if (column_value_expr->GetTupleIdx() == 0 && column_value_expr->GetColIdx() == idx) {
            continue;
          }
        }
        is_identical = false;
        break;
      }
      if (is_identical) {
        auto plan = child_plan->CloneWithChildren(child_plan->GetChildren());
        plan->output_schema_ = std::make_shared<Schema>(projection_schema);
        return plan;
      }
    }
  }
  return plan->CloneWithChildren(std::move(children));
}

}  // namespace bustub
