#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "binder/tokens.h"
#include "catalog/catalog.h"
#include "catalog/column.h"

namespace bustub {

class BoundStatement;
class SelectStatement;
class AbstractPlanNode;
class BoundExpression;
class BoundTableRef;

/**
 * The planner takes a bound statement, and transforms it into the BusTub plan tree.
 * The plan tree will be taken by the execution engine to execute the statement.
 */
class Planner {
 public:
  explicit Planner(const Catalog &catalog) : catalog_(catalog) {}

  // The following parts are undocumented. One `PlanXXX` functions simply corresponds to a
  // bound thing in the binder.

  void PlanQuery(const BoundStatement &statement);

  auto PlanSelect(const SelectStatement &statement) -> std::unique_ptr<AbstractPlanNode>;

  auto PlanTableRef(const BoundTableRef &table_ref) -> std::unique_ptr<AbstractPlanNode>;

  auto PlanExpression(const BoundExpression &expr, const AbstractPlanNode &child)
      -> std::unique_ptr<AbstractExpression>;

  /** the root plan node of the plan tree */
  std::unique_ptr<AbstractPlanNode> plan_;

 private:
  // The collection of allocated things, owned by the planner.
  // TODO(chi): refactor ALL OF THE ARGS TO UNIQUE_PTR/SHARED_PTR instead of guessing about the ownership.

  auto SavePlanNode(std::unique_ptr<AbstractPlanNode> plan_node) -> const AbstractPlanNode * {
    allocated_plan_nodes_.emplace_back(std::move(plan_node));
    return allocated_plan_nodes_.back().get();
  }

  auto SaveSchema(std::unique_ptr<Schema> schema) -> const Schema * {
    allocated_output_schemas_.emplace_back(std::move(schema));
    return allocated_output_schemas_.back().get();
  }

  auto SaveExpression(std::unique_ptr<AbstractExpression> expression) -> const AbstractExpression * {
    allocated_expressions_.emplace_back(std::move(expression));
    return allocated_expressions_.back().get();
  }

  std::vector<std::unique_ptr<Schema>> allocated_output_schemas_;

  std::vector<std::unique_ptr<AbstractPlanNode>> allocated_plan_nodes_;

  std::vector<std::unique_ptr<AbstractExpression>> allocated_expressions_;

  /** Catalog will be used during the planning process. SHOULD ONLY BE USED IN
   * CODE PATH OF `PlanQuery`, otherwise it's a dangling reference.
   */
  const Catalog &catalog_;
};

}  // namespace bustub
