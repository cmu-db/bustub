#pragma once

#include <memory>
#include <string>
#include <utility>

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
  explicit BoundSubqueryRef(std::unique_ptr<SelectStatement> subquery, std::string alias)
      : BoundTableRef(TableReferenceType::SUBQUERY), subquery_(std::move(subquery)), alias_(std::move(alias)) {}

  auto ToString() const -> std::string override;

  /** Subquery. */
  std::unique_ptr<SelectStatement> subquery_;

  /** Alias. */
  std::string alias_;
};
}  // namespace bustub
