//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_expression_list_ref.h
//
// Identification: src/include/binder/table_ref/bound_expression_list_ref.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

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
  explicit BoundExpressionListRef(std::vector<std::vector<std::unique_ptr<BoundExpression>>> values,
                                  std::string identifier)
      : BoundTableRef(TableReferenceType::EXPRESSION_LIST),
        values_(std::move(values)),
        identifier_(std::move(identifier)) {}

  auto ToString() const -> std::string override;

  /** The value list */
  std::vector<std::vector<std::unique_ptr<BoundExpression>>> values_;

  /** A unique identifier for this values list, so that planner / binder can work correctly. */
  std::string identifier_;
};
}  // namespace bustub
