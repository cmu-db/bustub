#include <algorithm>
#include <memory>
#include <optional>
#include <tuple>
#include "catalog/column.h"
#include "catalog/schema.h"
#include "common/exception.h"
#include "common/macros.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/comparison_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/abstract_plan.h"
#include "execution/plans/filter_plan.h"
#include "execution/plans/hash_join_plan.h"
#include "execution/plans/nested_index_join_plan.h"
#include "execution/plans/nested_loop_join_plan.h"
#include "execution/plans/projection_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "optimizer/optimizer.h"
#include "type/type_id.h"

namespace bustub {

auto Optimizer::MatchIndex(const std::string &table_name, uint32_t index_key_idx)
    -> std::optional<std::tuple<index_oid_t, std::string>> {
  const auto key_attrs = std::vector{index_key_idx};
  for (const auto *index_info : catalog_.GetTableIndexes(table_name)) {
    if (key_attrs == index_info->index_->GetKeyAttrs()) {
      return std::make_optional(std::make_tuple(index_info->index_oid_, index_info->name_));
    }
  }
  return std::nullopt;
}

auto Optimizer::OptimizeNLJAsIndexJoin(const AbstractPlanNodeRef &plan) -> AbstractPlanNodeRef {
  std::vector<AbstractPlanNodeRef> children;
  for (const auto &child : plan->GetChildren()) {
    children.emplace_back(OptimizeNLJAsIndexJoin(child));
  }
  auto optimized_plan = plan->CloneWithChildren(std::move(children));

  if (optimized_plan->GetType() == PlanType::NestedLoopJoin) {
    const auto &nlj_plan = dynamic_cast<const NestedLoopJoinPlanNode &>(*optimized_plan);
    // Has exactly two children
    BUSTUB_ENSURE(nlj_plan.children_.size() == 2, "NLJ should have exactly 2 children.");
    // Check if expr is equal condition where one is for the left table, and one is for the right table.
    if (const auto *expr = dynamic_cast<const ComparisonExpression *>(nlj_plan.Predicate().get()); expr != nullptr) {
      if (expr->comp_type_ == ComparisonType::Equal) {
        if (const auto *left_expr = dynamic_cast<const ColumnValueExpression *>(expr->children_[0].get());
            left_expr != nullptr) {
          if (const auto *right_expr = dynamic_cast<const ColumnValueExpression *>(expr->children_[1].get());
              right_expr != nullptr) {
            // Ensure both exprs have tuple_id == 0
            auto left_expr_tuple_0 =
                std::make_shared<ColumnValueExpression>(0, left_expr->GetColIdx(), left_expr->GetReturnType());
            auto right_expr_tuple_0 =
                std::make_shared<ColumnValueExpression>(0, right_expr->GetColIdx(), right_expr->GetReturnType());
            // Now it's in form of <column_expr> = <column_expr>. Let's match an index for them.

            // Ensure right child is table scan
            if (nlj_plan.GetRightPlan()->GetType() == PlanType::SeqScan) {
              const auto &right_seq_scan = dynamic_cast<const SeqScanPlanNode &>(*nlj_plan.GetRightPlan());
              if (left_expr->GetTupleIdx() == 0 && right_expr->GetTupleIdx() == 1) {
                if (auto index = MatchIndex(right_seq_scan.table_name_, right_expr->GetColIdx());
                    index != std::nullopt) {
                  auto [index_oid, index_name] = *index;
                  return std::make_shared<NestedIndexJoinPlanNode>(
                      nlj_plan.output_schema_, nlj_plan.GetLeftPlan(), std::move(left_expr_tuple_0),
                      right_seq_scan.GetTableOid(), index_oid, std::move(index_name), right_seq_scan.table_name_,
                      right_seq_scan.output_schema_, nlj_plan.GetJoinType());
                }
              }
              if (left_expr->GetTupleIdx() == 1 && right_expr->GetTupleIdx() == 0) {
                if (auto index = MatchIndex(right_seq_scan.table_name_, left_expr->GetColIdx());
                    index != std::nullopt) {
                  auto [index_oid, index_name] = *index;
                  return std::make_shared<NestedIndexJoinPlanNode>(
                      nlj_plan.output_schema_, nlj_plan.GetLeftPlan(), std::move(right_expr_tuple_0),
                      right_seq_scan.GetTableOid(), index_oid, std::move(index_name), right_seq_scan.table_name_,
                      right_seq_scan.output_schema_, nlj_plan.GetJoinType());
                }
              }
            }
          }
        }
      }
    }
  }

  return optimized_plan;
}

}  // namespace bustub
