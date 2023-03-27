#pragma once

#include <string>
#include <utility>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "fmt/core.h"

namespace bustub {

class CopyFromPlanNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new physical copy file plan node.
   * @param output the output format of this scan plan node
   */
  explicit CopyFromPlanNode(SchemaRef output, table_oid_t table_oid, uint8_t file_type)
      : AbstractPlanNode(std::move(output), {}), table_oid_(table_oid), file_type_(file_type) {}

  /** @return The type of the plan node */
  auto GetType() const -> PlanType override { return PlanType::CopyFrom; }

  /** @return The identifier of the table into which tuples are inserted */
  auto TableOid() const -> table_oid_t { return table_oid_; }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(CopyFromPlanNode);

  /** the relative file path */
  std::string file_path_;

  /** The table to be inserted into. */
  table_oid_t table_oid_;

  uint8_t file_type_;

 private:
  auto PlanNodeToString() const -> std::string override {
    return fmt::format("CopyFromFile {{ file_path={} }}", file_path_);
  }
};

}  // namespace bustub