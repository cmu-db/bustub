#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "binder/bound_table_ref.h"
#include "catalog/schema.h"
#include "fmt/core.h"
#include "type/type.h"

namespace bustub {

class BoundExpression;

/**
 * A bound table ref type for `values` clause.
 */
class BoundExpressionListRef : public BoundTableRef {
 public:
  explicit BoundExpressionListRef(std::vector<std::vector<std::unique_ptr<BoundExpression>>> values)
      : BoundTableRef(TableReferenceType::EXPRESSION_LIST), values_(std::move(values)) {}

  auto ToString() const -> std::string override;

  /** The value list */
  std::vector<std::vector<std::unique_ptr<BoundExpression>>> values_;
};
}  // namespace bustub
