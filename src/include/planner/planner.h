#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

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
  explicit Planner(const Catalog &catalog) : catalog_(catalog) {}

  void PlanQuery(const BoundStatement &statement);

  auto PlanSelect(const SelectStatement &statement) -> std::unique_ptr<AbstractPlanNode>;

  auto PlanTableRef(const BoundTableRef &table_ref) -> std::unique_ptr<AbstractPlanNode>;

  auto PlanExpression(const BoundExpression &expr, const AbstractPlanNode &child)
      -> std::unique_ptr<AbstractExpression>;

  std::unique_ptr<AbstractPlanNode> plan_;

  auto SavePlanNode(std::unique_ptr<AbstractPlanNode> plan_node) -> const AbstractPlanNode * {
    allocated_plan_nodes_.emplace_back(move(plan_node));
    return allocated_plan_nodes_.back().get();
  }

  auto SaveSchema(std::unique_ptr<Schema> schema) -> const Schema * {
    allocated_output_schemas_.emplace_back(move(schema));
    return allocated_output_schemas_.back().get();
  }

  auto SaveExpression(std::unique_ptr<AbstractExpression> expression) -> const AbstractExpression * {
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
