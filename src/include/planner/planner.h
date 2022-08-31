#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <string>

#include "binder/bound_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/tokens.h"
#include "catalog/catalog.h"
#include "catalog/column.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"

namespace bustub {

class Planner {
 public:
  Planner(const Catalog &catalog) : catalog_(catalog) {}

  void PlanQuery(const BoundStatement &statement);

  unique_ptr<AbstractPlanNode> PlanSelect(const SelectStatement &statement);

  unique_ptr<AbstractPlanNode> PlanTableRef(const BoundTableRef &table_ref);

  unique_ptr<AbstractExpression> PlanExpression(const BoundExpression &expr, const AbstractPlanNode &child);

  std::unique_ptr<AbstractPlanNode> plan_;

  const AbstractPlanNode *SavePlanNode(std::unique_ptr<AbstractPlanNode> plan_node) {
    allocated_plan_nodes_.emplace_back(move(plan_node));
    return allocated_plan_nodes_.back().get();
  }

  const Schema *SaveSchema(std::unique_ptr<Schema> schema) {
    allocated_output_schemas_.emplace_back(move(schema));
    return allocated_output_schemas_.back().get();
  }

  const AbstractExpression *SaveExpression(std::unique_ptr<AbstractExpression> expression) {
    allocated_expressions_.emplace_back(move(expression));
    return allocated_expressions_.back().get();
  }

  /** The collection of allocated schemas, owned by the planner.
   * TODO(chi): refactor ALL OF THE ARGS TO UNIQUE_PTR/SHARED_PTR instead of guessing about the ownership.
   */
  std::vector<std::unique_ptr<Schema>> allocated_output_schemas_;

  std::vector<std::unique_ptr<AbstractPlanNode>> allocated_plan_nodes_;

  std::vector<std::unique_ptr<AbstractExpression>> allocated_expressions_;

  const Catalog &catalog_;
};

}  // namespace bustub
