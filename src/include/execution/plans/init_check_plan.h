//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// init_check_plan.h
//
// Identification: src/include/execution/plans/init_check_plan.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <utility>

#include "execution/plans/abstract_plan.h"
#include "fmt/format.h"

namespace bustub {

/**
 * InitCheck counts the number of init calls by its child executor.
 */
class InitCheckPlanNode : public AbstractPlanNode {
 public:
  /**
   * Construct a new InitCheckPlanNode instance.
   * @param output The output schema of this init check plan node
   * @param child The child plan from which tuples are obtained
   */
  InitCheckPlanNode(SchemaRef output, AbstractPlanNodeRef child)
      : AbstractPlanNode(std::move(output), {std::move(child)}) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::InitCheck; }

  /** @return The child plan node */
  auto GetChildPlan() const -> AbstractPlanNodeRef {
    BUSTUB_ASSERT(GetChildren().size() == 1, "InitCheck should have at most one child plan.");
    return GetChildAt(0);
  }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(InitCheckPlanNode);
};

}  // namespace bustub
