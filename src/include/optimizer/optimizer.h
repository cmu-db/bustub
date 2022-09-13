#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "catalog/catalog.h"

namespace bustub {

class AbstractPlanNode;

/**
 * The optimizer takes an `AbstractPlanNode` and outputs an optimized `AbstractPlanNode`.
 */
class Optimizer {
 public:
  explicit Optimizer(const Catalog &catalog) : catalog_(catalog) {}

  auto Optimize(AbstractPlanNode *plan) -> AbstractPlanNode *;

 private:
  /** Catalog will be used during the planning process. USERS SHOULD ENSURE IT OUTLIVES
   * OPTIMIZER, otherwise it's a dangling reference.
   */
  [[maybe_unused]] const Catalog &catalog_;
};

}  // namespace bustub
