
#include <memory>
#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/expressions/bound_constant.h"
#include "binder/statement/set_show_statement.h"
#include "common/exception.h"
#include "nodes/parsenodes.hpp"
namespace bustub {

auto Binder::BindVariableSet(duckdb_libpgquery::PGVariableSetStmt *stmt) -> std::unique_ptr<VariableSetStatement> {
  auto expr = BindExpressionList(stmt->args);
  if (expr.size() != 1) {
    throw bustub::NotImplementedException("Only exactly one arg is supported");
  }
  if (expr[0]->type_ != ExpressionType::CONSTANT) {
    throw bustub::NotImplementedException("Only constant is supported");
  }
  const auto &const_expr = dynamic_cast<const BoundConstant &>(*expr[0]);
  return std::make_unique<VariableSetStatement>(stmt->name, const_expr.val_.ToString());
}

auto Binder::BindVariableShow(duckdb_libpgquery::PGVariableShowStmt *stmt) -> std::unique_ptr<VariableShowStatement> {
  return std::make_unique<VariableShowStatement>(stmt->name);
}

auto Binder::BindTransaction(duckdb_libpgquery::PGTransactionStmt *stmt) -> std::unique_ptr<TransactionStatement> {
  switch (stmt->kind) {
    case duckdb_libpgquery::PG_TRANS_STMT_COMMIT:
      return std::make_unique<TransactionStatement>("commit");
    case duckdb_libpgquery::PG_TRANS_STMT_ROLLBACK:
      return std::make_unique<TransactionStatement>("abort");
    case duckdb_libpgquery::PG_TRANS_STMT_BEGIN:
      return std::make_unique<TransactionStatement>("begin");
    default:
      throw bustub::NotImplementedException("unsupported txn statement kind");
  }
}

}  // namespace bustub
