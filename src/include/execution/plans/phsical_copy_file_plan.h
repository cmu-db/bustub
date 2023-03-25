#pragma once

#include <string>
#include <utility>

#include "catalog/catalog.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "fmt/core.h"

namespace bustub {

class PhysicalCopyFileNode : public AbstractPlanNode {
 public:
  /**
   * Creates a new physical copy file plan node.
   * @param output the output format of this scan plan node
   */
  explicit PhysicalCopyFileNode(SchemaRef output) : AbstractPlanNode(std::move(output), {}) {}

  auto GetType() const -> PlanType override { return PlanType::PhysicalCopyFile; }

  BUSTUB_PLAN_NODE_CLONE_WITH_CHILDREN(PhysicalCopyFileNode);

 protected:
  auto PlanNodeToString() const -> std::string override {
    return fmt::format("PhysicalCopy {{ file_path={} }}", path_);
  }

 private:
  /** the relative file path */
  std::string path_;

  // column_type

  // file_format(specify csv format)
};

}  // namespace bustub