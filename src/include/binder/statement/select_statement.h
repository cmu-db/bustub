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

#include "binder/bound_expression.h"
#include "binder/bound_tableref.h"
#include "binder/parser.h"

namespace bustub {

class SelectStatement : public SQLStatement {
 public:
  explicit SelectStatement(const Parser &parser, duckdb_libpgquery::PGSelectStmt *pg_stmt);

  static auto BindSelectList(duckdb_libpgquery::PGList *list) -> vector<unique_ptr<BoundExpression>>;

  static auto BindExpression(duckdb_libpgquery::PGNode *node) -> unique_ptr<BoundExpression>;

  static auto BindConstant(duckdb_libpgquery::PGAConst *node) -> unique_ptr<BoundExpression>;

  static auto BindColumnRef(duckdb_libpgquery::PGColumnRef *node) -> unique_ptr<BoundExpression>;

  static auto BindResTarget(duckdb_libpgquery::PGResTarget *root) -> unique_ptr<BoundExpression>;

  unique_ptr<BoundTableRef> table_;
  vector<unique_ptr<BoundExpression>> select_list_;

  auto ToString() const -> string override;
};

}  // namespace bustub
