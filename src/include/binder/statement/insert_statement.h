//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// insert_statement.h
//
// Identification: src/include/binder/statement/insert_statement.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "binder/bound_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "catalog/column.h"
#include "type/value.h"

namespace bustub {

class SelectStatement;

class InsertStatement : public BoundStatement {
 public:
  explicit InsertStatement(std::unique_ptr<BoundBaseTableRef> table, std::unique_ptr<SelectStatement> select);

  std::unique_ptr<BoundBaseTableRef> table_;

  std::unique_ptr<SelectStatement> select_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
