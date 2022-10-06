//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/select_statement.h
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/table_ref/bound_subquery_ref.h"

namespace bustub {

class Catalog;

class SelectStatement : public BoundStatement {
 public:
  explicit SelectStatement(std::unique_ptr<BoundTableRef> table,
                           std::vector<std::unique_ptr<BoundExpression>> select_list,
                           std::unique_ptr<BoundExpression> where,
                           std::vector<std::unique_ptr<BoundExpression>> group_by,
                           std::unique_ptr<BoundExpression> having, std::unique_ptr<BoundExpression> limit_count,
                           std::unique_ptr<BoundExpression> limit_offset,
                           std::vector<std::unique_ptr<BoundOrderBy>> sort, CTEList ctes, bool is_distinct)
      : BoundStatement(StatementType::SELECT_STATEMENT),
        table_(std::move(table)),
        select_list_(std::move(select_list)),
        where_(std::move(where)),
        group_by_(std::move(group_by)),
        having_(std::move(having)),
        limit_count_(std::move(limit_count)),
        limit_offset_(std::move(limit_offset)),
        sort_(std::move(sort)),
        ctes_(std::move(ctes)),
        is_distinct_(is_distinct) {}

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

  /** Bound LIMIT clause. */
  std::unique_ptr<BoundExpression> limit_count_;

  /** Bound OFFSET clause. */
  std::unique_ptr<BoundExpression> limit_offset_;

  /** Bound ORDER BY clause. */
  std::vector<std::unique_ptr<BoundOrderBy>> sort_;

  /** Bound CTE. */
  CTEList ctes_;

  /** Is SELECT DISTINCT */
  bool is_distinct_;

  auto ToString() const -> std::string override;
};

}  // namespace bustub
