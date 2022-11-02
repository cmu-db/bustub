//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/insert_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "binder/tokens.h"
#include "catalog/column.h"
#include "common/enums/statement_type.h"
#include "type/value.h"

namespace bustub {

class UpdateStatement : public BoundStatement {
 public:
  explicit UpdateStatement(
      std::unique_ptr<BoundBaseTableRef> table, std::unique_ptr<BoundExpression> filter_expr,
      std::vector<std::pair<std::unique_ptr<BoundColumnRef>, std::unique_ptr<BoundExpression>>> target_expr);

  std::unique_ptr<BoundBaseTableRef> table_;

  std::unique_ptr<BoundExpression> filter_expr_;

  std::vector<std::pair<std::unique_ptr<BoundColumnRef>, std::unique_ptr<BoundExpression>>> target_expr_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
