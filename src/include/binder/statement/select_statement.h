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

#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_table_ref.h"

namespace bustub {

class SelectStatement : public SQLStatement {
 public:
  explicit SelectStatement(const Catalog &catalog, duckdb_libpgquery::PGSelectStmt *pg_stmt);

  // The following parts are undocumented. One `BindXXX` functions simply corresponds to a
  // node type in the Postgres parse tree.

  void BindSelectList(duckdb_libpgquery::PGList *list);

  void BindWhere(duckdb_libpgquery::PGNode *root);

  void BindGroupBy(duckdb_libpgquery::PGList *list);

  void BindHaving(duckdb_libpgquery::PGNode *root);

  auto BindExpression(duckdb_libpgquery::PGNode *node) -> unique_ptr<BoundExpression>;

  auto BindConstant(duckdb_libpgquery::PGAConst *node) -> unique_ptr<BoundExpression>;

  auto BindColumnRef(duckdb_libpgquery::PGColumnRef *node) -> unique_ptr<BoundExpression>;

  auto BindResTarget(duckdb_libpgquery::PGResTarget *root) -> unique_ptr<BoundExpression>;

  auto BindStar(duckdb_libpgquery::PGAStar *node) -> unique_ptr<BoundExpression>;

  auto BindFuncCall(duckdb_libpgquery::PGFuncCall *root) -> unique_ptr<BoundExpression>;

  auto BindAExpr(duckdb_libpgquery::PGAExpr *root) -> unique_ptr<BoundExpression>;

  auto ResolveColumn(const string &col_name) -> unique_ptr<BoundExpression>;

  auto ResolveColumnWithTable(const string &table_name, const string &col_name) -> unique_ptr<BoundExpression>;

  /** bound FROM clause */
  unique_ptr<BoundTableRef> table_;

  /** bound select list */
  vector<unique_ptr<BoundExpression>> select_list_;

  /** bound WHERE clause */
  unique_ptr<BoundExpression> where_;

  /** bound GROUP BY clause */
  vector<unique_ptr<BoundExpression>> group_by_;

  /** bound HAVING clause */
  unique_ptr<BoundExpression> having_;

  /** convert the bound statement to string */
  auto ToString() const -> string override;

 private:
  /** Catalog will be used during the binding process. SHOULD ONLY BE USED IN
   * CODE PATH OF CONSTRUCTORS, otherwise it's a dangling reference.
   */
  const Catalog &catalog_;
};

}  // namespace bustub
