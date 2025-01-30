//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_subquery_ref.h
//
// Identification: src/include/binder/table_ref/bound_subquery_ref.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "binder/bound_expression.h"
#include "binder/bound_table_ref.h"
#include "fmt/format.h"

namespace bustub {

class SelectStatement;

/**
 * A subquery. e.g., `SELECT * FROM (SELECT * FROM t1)`, where `(SELECT * FROM t1)` is `BoundSubqueryRef`.
 */
class BoundSubqueryRef : public BoundTableRef {
 public:
  explicit BoundSubqueryRef(std::unique_ptr<SelectStatement> subquery,
                            std::vector<std::vector<std::string>> select_list_name, std::string alias)
      : BoundTableRef(TableReferenceType::SUBQUERY),
        subquery_(std::move(subquery)),
        select_list_name_(std::move(select_list_name)),
        alias_(std::move(alias)) {}

  auto ToString() const -> std::string override;

  /** Subquery. */
  std::unique_ptr<SelectStatement> subquery_;

  /** Name of each item in the select list. */
  std::vector<std::vector<std::string>> select_list_name_;

  /** Alias. */
  std::string alias_;
};

using CTEList = std::vector<std::unique_ptr<BoundSubqueryRef>>;

}  // namespace bustub
