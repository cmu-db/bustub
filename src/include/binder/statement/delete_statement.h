//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/delete_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>

#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "catalog/column.h"

namespace bustub {

class DeleteStatement : public BoundStatement {
 public:
  explicit DeleteStatement(std::unique_ptr<BoundBaseTableRef> table, std::unique_ptr<BoundExpression> expr);

  std::unique_ptr<BoundBaseTableRef> table_;

  std::unique_ptr<BoundExpression> expr_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
