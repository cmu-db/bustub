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
#include "binder/simplified_token.h"
#include "catalog/column.h"

namespace duckdb_libpgquery {
struct PGList;
struct PGSelectStmt;
struct PGAConst;
struct PGAStar;
struct PGFuncCall;
struct PGNode;
struct PGColumnRef;
struct PGResTarget;
struct PGAExpr;
struct PGJoinExpr;
}  // namespace duckdb_libpgquery

namespace bustub {

class Catalog;
class BoundTableRef;
class BoundExpression;

class SelectStatement : public BoundStatement {
 public:
  explicit SelectStatement(const Catalog &catalog, duckdb_libpgquery::PGSelectStmt *pg_stmt);

  // The following parts are undocumented. One `BindXXX` functions simply corresponds to a
  // node type in the Postgres parse tree.

  void BindSelectList(duckdb_libpgquery::PGList *list);

  void BindWhere(duckdb_libpgquery::PGNode *root);

  void BindGroupBy(duckdb_libpgquery::PGList *list);

  void BindHaving(duckdb_libpgquery::PGNode *root);

  auto BindExpression(duckdb_libpgquery::PGNode *node) -> std::unique_ptr<BoundExpression>;

  auto BindExpressionWithScope(duckdb_libpgquery::PGNode *node, const BoundTableRef &scope)
      -> std::unique_ptr<BoundExpression>;

  auto BindConstant(duckdb_libpgquery::PGAConst *node) -> std::unique_ptr<BoundExpression>;

  auto BindColumnRef(duckdb_libpgquery::PGColumnRef *node, const bustub::BoundTableRef &scope)
      -> std::unique_ptr<BoundExpression>;

  auto BindResTarget(duckdb_libpgquery::PGResTarget *root) -> std::unique_ptr<BoundExpression>;

  auto BindStar(duckdb_libpgquery::PGAStar *node) -> std::unique_ptr<BoundExpression>;

  auto BindFuncCall(duckdb_libpgquery::PGFuncCall *root) -> std::unique_ptr<BoundExpression>;

  auto BindAExpr(duckdb_libpgquery::PGAExpr *root, const bustub::BoundTableRef &scope)
      -> std::unique_ptr<BoundExpression>;

  auto BindFrom(duckdb_libpgquery::PGList *list) -> std::unique_ptr<BoundTableRef>;

  auto BindTableRef(duckdb_libpgquery::PGNode *node) -> std::unique_ptr<BoundTableRef>;

  auto BindJoin(duckdb_libpgquery::PGJoinExpr *root) -> std::unique_ptr<BoundTableRef>;

  void AddAllColumnsToList(const BoundTableRef &table_ref);

  auto ResolveColumn(const BoundTableRef &table_ref, const std::vector<std::string> &col_name)
      -> std::unique_ptr<BoundExpression>;

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

 private:
  /** Catalog will be used during the binding process. SHOULD ONLY BE USED IN
   * CODE PATH OF CONSTRUCTORS, otherwise it's a dangling reference.
   */
  const Catalog &catalog_;
};

}  // namespace bustub
