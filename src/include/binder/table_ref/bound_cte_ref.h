//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bound_cte_ref.h
//
// Identification: src/include/binder/table_ref/bound_cte_ref.h
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
 * A CTE. e.g., `WITH (select 1) x SELECT * FROM x`, where `x` is `BoundCTERef`.
 */
class BoundCTERef : public BoundTableRef {
 public:
  explicit BoundCTERef(std::string cte_name, std::string alias)
      : BoundTableRef(TableReferenceType::CTE), cte_name_(std::move(cte_name)), alias_(std::move(alias)) {}

  auto ToString() const -> std::string override;

  /** CTE name. */
  std::string cte_name_;

  /** Alias. */
  std::string alias_;
};

}  // namespace bustub
