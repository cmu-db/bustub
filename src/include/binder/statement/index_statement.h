//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// index_statement.h
//
// Identification: src/include/binder/statement/index_statement.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "binder/bound_statement.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "catalog/column.h"

namespace bustub {

class IndexStatement : public BoundStatement {
 public:
  explicit IndexStatement(std::string index_name, std::unique_ptr<BoundBaseTableRef> table,
                          std::vector<std::unique_ptr<BoundColumnRef>> cols, std::string index_type,
                          std::vector<std::string> col_options, std::vector<std::pair<std::string, int>> options);

  /** Name of the index */
  std::string index_name_;

  /** Create on which table */
  std::unique_ptr<BoundBaseTableRef> table_;

  /** Name of the columns */
  std::vector<std::unique_ptr<BoundColumnRef>> cols_;

  /** Using */
  std::string index_type_;

  std::vector<std::string> col_options_;
  std::vector<std::pair<std::string, int>> options_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
