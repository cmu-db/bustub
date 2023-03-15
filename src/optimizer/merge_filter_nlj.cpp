#include <algorithm>
#include <memory>
#include "catalog/column.h"
#include "catalog/schema.h"
#include "common/exception.h"
#include "common/macros.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/nested_loop_join_plan.h"
#include "execution/plans/projection_plan.h"
#include "optimizer/optimizer.h"
#include "type/type_id.h"

namespace bustub {

auto Optimizer::RewriteExpressionForJoin(const AbstractExpressionRef &expr, size_t left_column_cnt,
                                         size_t right_column_cnt) -> AbstractExpressionRef {
  std::vector<AbstractExpressionRef> children;
  for (const auto &child : expr->GetChildren()) {
    children.emplace_back(RewriteExpressionForJoin(child, left_column_cnt, right_column_cnt));
  }
  if (const auto *column_value_expr = dynamic_cast<const ColumnValueExpression *>(expr.get());
      column_value_expr != nullptr) {
    BUSTUB_ENSURE(column_value_expr->GetTupleIdx() == 0, "tuple_idx cannot be value other than 0 before this stage.")
    auto col_idx = column_value_expr->GetColIdx();
    if (col_idx < left_column_cnt) {
      return std::make_shared<ColumnValueExpression>(0, col_idx, column_value_expr->GetReturnType());
    }
    if (col_idx >= left_column_cnt && col_idx < left_column_cnt + right_column_cnt) {
      return std::make_shared<ColumnValueExpression>(1, col_idx - left_column_cnt, column_value_expr->GetReturnType());
    }
    throw bustub::Exception("col_idx not in range");
  }
  return expr->CloneWithChildren(children);
}

auto Optimizer::IsPredicateTrue(const AbstractExpressionRef &expr) -> bool {
  if (const auto *const_expr = dynamic_cast<const ConstantValueExpression *>(expr.get()); const_expr != nullptr) {
    return const_expr->val_.CastAs(TypeId::BOOLEAN).GetAs<bool>();
  }
  return false;
}

auto Optimizer::OptimizeMergeFilterNLJ(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  std::vector<AbstractPlanNodeRef> children;
  for (const auto &child : plan->GetChildren()) {
    children.emplace_back(OptimizeMergeFilterNLJ(child));
  }
  auto optimized_plan = plan->CloneWithChildren(std::move(children));

  if (optimized_plan->GetType() == PlanType::Filter) {
    const auto &filter_plan = dynamic_cast<const FilterPlanNode &>(*optimized_plan);
    // Has exactly one child
    BUSTUB_ENSURE(optimized_plan->children_.size() == 1, "Filter with multiple children?? Impossible!");
    const auto &child_plan = optimized_plan->children_[0];
    if (child_plan->GetType() == PlanType::NestedLoopJoin) {
      const auto &nlj_plan = dynamic_cast<const NestedLoopJoinPlanNode &>(*child_plan);
      // Has exactly two children
      BUSTUB_ENSURE(child_plan->GetChildren().size() == 2, "NLJ should have exactly 2 children.");

      if (IsPredicateTrue(nlj_plan.Predicate())) {
        // Only rewrite when NLJ has always true predicate.
        return std::make_shared<NestedLoopJoinPlanNode>(
            filter_plan.output_schema_, nlj_plan.GetLeftPlan(), nlj_plan.GetRightPlan(),
            RewriteExpressionForJoin(filter_plan.GetPredicate(),
                                     nlj_plan.GetLeftPlan()->OutputSchema().GetColumnCount(),
                                     nlj_plan.GetRightPlan()->OutputSchema().GetColumnCount()),
            nlj_plan.GetJoinType());
      }
    }
  }
  return optimized_plan;
}

}  // namespace bustub
