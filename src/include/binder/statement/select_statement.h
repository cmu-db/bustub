//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/select_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "binder/bound_statement.h"

namespace bustub {

class Catalog;
class BoundTableRef;
class BoundExpression;

class SelectStatement : public BoundStatement {
 public:
  explicit SelectStatement(std::unique_ptr<BoundTableRef> table,
                           std::vector<std::unique_ptr<BoundExpression>> select_list,
                           std::unique_ptr<BoundExpression> where,
                           std::vector<std::unique_ptr<BoundExpression>> group_by,
                           std::unique_ptr<BoundExpression> having);
  /** Bound FROM clause. */
  std::unique_ptr<BoundTableRef> table_;

  /** Bound SELECT list. */
  std::vector<std::unique_ptr<BoundExpression>> select_list_;

  /** Bound WHERE clause. */
  std::unique_ptr<BoundExpression> where_;

  /** Bound GROUP BY clause. */
  std::vector<std::unique_ptr<BoundExpression>> group_by_;

  /** Bound HAVING clause. */
  std::unique_ptr<BoundExpression> having_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
