#include "binder/statement/select_statement.h"
#include <fmt/format.h>
#include "binder/expressions/bound_column_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/table_ref/bound_base_table_ref.h"

namespace bustub {

SelectStatement::SelectStatement(const Parser &parser, duckdb_libpgquery::PGSelectStmt *pg_stmt)
    : SQLStatement(StatementType::SELECT_STATEMENT), table_(make_unique<BoundTableRef>()) {
  // bind from clause
  if (pg_stmt->fromClause != nullptr) {
    // Extract the table name from the FROM clause.
    for (auto c = pg_stmt->fromClause->head; c != nullptr; c = lnext(c)) {
      auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
      switch (node->type) {
        case duckdb_libpgquery::T_PGRangeVar: {
          auto *table_ref = reinterpret_cast<duckdb_libpgquery::PGRangeVar *>(node);
          table_ = make_unique<BoundBaseTableRef>(table_ref->relname);
          break;
        }
        default:
          throw Exception(fmt::format("unsupported node type: {}", Parser::NodetypeToString(node->type)));
      }
    }
  } else {
    table_ = make_unique<BoundTableRef>(TableReferenceType::EMPTY);
  }

  // bind select list
  if (pg_stmt->targetList != nullptr) {
    select_list_ = BindSelectList(pg_stmt->targetList);
  } else {
    throw Exception("no select list");
  }

  // TODO(chi): If there are any extra args (e.g. group by, having) not supported by the binder,
  // we should have thrown an exception. However, this is too tedious to implement (need to check
  // every field manually). Therefore, I'd prefer warning users that the binder is not complete
  // in project write-ups / READMEs.
}

auto SelectStatement::ToString() const -> string {
  vector<string> columns;
  for (const auto &expr : select_list_) {
    columns.push_back(fmt::format("{}", expr));
  }
  return fmt::format("Select {{ table={}, columns=[{}] }}", table_, fmt::join(columns, ", "));
}

auto SelectStatement::BindSelectList(duckdb_libpgquery::PGList *list) -> vector<unique_ptr<BoundExpression>> {
  vector<unique_ptr<BoundExpression>> exprs;

  for (auto node = list->head; node != nullptr; node = lnext(node)) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value);

    auto expr = BindExpression(target);

    exprs.push_back(move(expr));
  }

  return exprs;
}

auto SelectStatement::BindConstant(duckdb_libpgquery::PGAConst *node) -> unique_ptr<BoundExpression> {
  auto bound_val = Value(TypeId::INTEGER);
  const auto &val = node->val;
  switch (val.type) {
    case duckdb_libpgquery::T_PGInteger:
      assert(val.val.ival <= BUSTUB_INT32_MAX);
      bound_val = Value(TypeId::INTEGER, static_cast<int32_t>(val.val.ival));
      break;
    default:
      throw Exception(fmt::format("unsupported pg value: {}", Parser::NodetypeToString(val.type)));
  }
  return make_unique<BoundConstant>(move(bound_val));
}

auto SelectStatement::BindColumnRef(duckdb_libpgquery::PGColumnRef *node) -> unique_ptr<BoundExpression> {
  auto fields = node->fields;
  auto head_node = static_cast<duckdb_libpgquery::PGNode *>(fields->head->data.ptr_value);
  switch (head_node->type) {
    case duckdb_libpgquery::T_PGString: {
      if (fields->length < 1) {
        throw Exception("Unexpected field length");
      }
      vector<string> column_names;
      for (auto node = fields->head; node != nullptr; node = node->next) {
        column_names.emplace_back(reinterpret_cast<duckdb_libpgquery::PGValue *>(node->data.ptr_value)->val.str);
      }
      if (column_names.size() != 1) {
        throw Exception(fmt::format("unsupported ColumnRef: multiple elements found"));
      }
      auto colref = make_unique<BoundColumnRef>("test", move(column_names[0]));
      return colref;
    }
    default:
      throw Exception("ColumnRef not implemented!");
  }
}

auto SelectStatement::BindResTarget(duckdb_libpgquery::PGResTarget *root) -> unique_ptr<BoundExpression> {
  assert(root);
  auto expr = BindExpression(root->val);
  if (!expr) {
    return nullptr;
  }
  // if (root->name) {
  //   expr->alias = string(root->name);
  // }
  return expr;
}

auto SelectStatement::BindExpression(duckdb_libpgquery::PGNode *node) -> unique_ptr<BoundExpression> {
  switch (node->type) {
    case duckdb_libpgquery::T_PGColumnRef:
      return BindColumnRef(reinterpret_cast<duckdb_libpgquery::PGColumnRef *>(node));
    case duckdb_libpgquery::T_PGAConst:
      return BindConstant(reinterpret_cast<duckdb_libpgquery::PGAConst *>(node));
    case duckdb_libpgquery::T_PGResTarget:
      return BindResTarget(reinterpret_cast<duckdb_libpgquery::PGResTarget *>(node));
    default:
      throw Exception(fmt::format("Expr of type {} not implemented\n", Parser::NodetypeToString(node->type)));
  }
  return make_unique<BoundExpression>();
}

}  // namespace bustub
