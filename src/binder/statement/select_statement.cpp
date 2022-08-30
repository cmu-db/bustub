#include "binder/statement/select_statement.h"
#include <fmt/format.h>
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/expressions/bound_star.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "catalog/catalog.h"
#include "common/util/string_util.h"

namespace bustub {

SelectStatement::SelectStatement(const Catalog &catalog, duckdb_libpgquery::PGSelectStmt *pg_stmt)
    : SQLStatement(StatementType::SELECT_STATEMENT),
      table_(make_unique<BoundTableRef>()),
      where_(make_unique<BoundExpression>()),
      having_(make_unique<BoundExpression>()),
      catalog_(catalog) {
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
    BindSelectList(pg_stmt->targetList);
  } else {
    throw Exception("no select list");
  }

  if (pg_stmt->whereClause != nullptr) {
    BindWhere(pg_stmt->whereClause);
  }

  if (pg_stmt->groupClause != nullptr) {
    BindGroupBy(pg_stmt->groupClause);
  }

  if (pg_stmt->havingClause != nullptr) {
    BindHaving(pg_stmt->havingClause);
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
  return fmt::format("Select {{ table={}, columns=[{}], groupBy=[{}], having={}, where={} }}", table_,
                     fmt::join(columns, ", "), fmt::join(group_by_, ", "), having_, where_);
}

void SelectStatement::BindSelectList(duckdb_libpgquery::PGList *list) {
  vector<unique_ptr<BoundExpression>> exprs;

  for (auto node = list->head; node != nullptr; node = lnext(node)) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value);

    auto expr = BindExpression(target);

    // Process `select *`
    if (expr->type_ == ExpressionType::STAR) {
      if (select_list_.empty()) {
        switch (table_->type_) {
          case TableReferenceType::BASE_TABLE: {
            auto base_table_ref = dynamic_cast<BoundBaseTableRef *>(table_.get());
            const auto &table_name = base_table_ref->table_;
            auto table_info = catalog_.GetTable(table_name);
            if (table_info == nullptr) {
              throw Exception(fmt::format("invalid table {}", table_name));
            }

            for (const auto &column : table_info->schema_.GetColumns()) {
              select_list_.push_back(make_unique<BoundColumnRef>(table_name, column.GetName()));
            }
            return;
          }
          default:
            throw Exception("select * cannot be used with this TableReferenceType");
        }
      } else {
        throw Exception("select * cannot have other expressions in list");
      }
    } else {
      select_list_.push_back(move(expr));
    }
  }
}

auto SelectStatement::BindConstant(duckdb_libpgquery::PGAConst *node) -> unique_ptr<BoundExpression> {
  assert(node);
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
  assert(node);
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
      if (column_names.size() == 1) {
        // select col
        return ResolveColumn(column_names[0]);
      }
      if (column_names.size() == 2) {
        // select table.col
        return ResolveColumnWithTable(column_names[0], column_names[1]);
      }
      throw Exception(fmt::format("unsupported ColumnRef: zero or multiple elements found"));
    }
    case duckdb_libpgquery::T_PGAStar: {
      return BindStar(reinterpret_cast<duckdb_libpgquery::PGAStar *>(head_node));
    }
    default:
      throw Exception(fmt::format("ColumnRef type {} not implemented!", Parser::NodetypeToString(head_node->type)));
  }
}

auto SelectStatement::BindResTarget(duckdb_libpgquery::PGResTarget *root) -> unique_ptr<BoundExpression> {
  assert(root);
  auto expr = BindExpression(root->val);
  if (!expr) {
    return nullptr;
  }
  return expr;
}

auto SelectStatement::BindStar(duckdb_libpgquery::PGAStar *node) -> unique_ptr<BoundExpression> {
  assert(node);
  return make_unique<BoundStar>();
}

auto SelectStatement::BindFuncCall(duckdb_libpgquery::PGFuncCall *root) -> unique_ptr<BoundExpression> {
  assert(root);
  auto name = root->funcname;
  auto function_name =
      StringUtil::Lower(reinterpret_cast<duckdb_libpgquery::PGValue *>(name->head->data.ptr_value)->val.str);

  vector<unique_ptr<BoundExpression>> children;
  if (root->args != nullptr) {
    for (auto node = root->args->head; node != nullptr; node = node->next) {
      auto child_expr = BindExpression(static_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value));
      children.push_back(move(child_expr));
    }
  }

  if (function_name == "min" || function_name == "max" || function_name == "first" || function_name == "last" ||
      function_name == "sum" || function_name == "count") {
    // bind function as agg call

    // rewrite count(*) to count_star()
    if (function_name == "count" && children.size() == 1 && children[0]->type_ == ExpressionType::STAR) {
      function_name = "count_star";
      children.clear();
    }

    return make_unique<BoundAggCall>(function_name, move(children));
  }
  throw Exception(fmt::format("unsupported func call {}", function_name));
}

auto SelectStatement::ResolveColumn(const string &col_name) -> unique_ptr<BoundExpression> {
  switch (table_->type_) {
    case TableReferenceType::BASE_TABLE: {
      auto base_table_ref = dynamic_cast<BoundBaseTableRef *>(table_.get());
      const auto &table_name = base_table_ref->table_;
      auto table_info = catalog_.GetTable(table_name);
      if (table_info == nullptr) {
        throw Exception(fmt::format("invalid table {}", table_name));
      }
      bool found = false;
      auto expr = make_unique<BoundExpression>();
      for (const auto &column : table_info->schema_.GetColumns()) {
        auto name = column.GetName();
        if (name == col_name) {
          if (!found) {
            expr = make_unique<BoundColumnRef>(table_name, name);
            found = true;
          } else {
            throw Exception(fmt::format("column {} appears in multiple tables", col_name));
          }
        }
      }
      if (found) {
        return expr;
      }
      throw Exception(fmt::format("column {} not found", col_name));
    }
    default:
      throw Exception("unsupported TableReferenceType");
  }
}

auto SelectStatement::ResolveColumnWithTable(const string &table_name, const string &col_name)
    -> unique_ptr<BoundExpression> {
  switch (table_->type_) {
    case TableReferenceType::BASE_TABLE: {
      auto base_table_ref = dynamic_cast<BoundBaseTableRef *>(table_.get());
      if (base_table_ref->table_ != table_name) {
        throw Exception("table not in from clause");
      }
      break;
    }
    default:
      throw Exception("unsupported TableReferenceType");
  }
  auto table_info = catalog_.GetTable(table_name);
  if (table_info == nullptr) {
    throw Exception(fmt::format("invalid table {}", table_name));
  }
  bool found = false;
  auto expr = make_unique<BoundExpression>();
  for (const auto &column : table_info->schema_.GetColumns()) {
    auto name = column.GetName();
    if (name == col_name) {
      if (!found) {
        expr = make_unique<BoundColumnRef>(table_name, name);
        found = true;
      } else {
        throw Exception(fmt::format("column {} appears in multiple tables", col_name));
      }
    }
  }
  if (found) {
    return expr;
  }
  throw Exception(fmt::format("column {} not found", col_name));
}

void SelectStatement::BindWhere(duckdb_libpgquery::PGNode *root) { where_ = BindExpression(root); }

void SelectStatement::BindGroupBy(duckdb_libpgquery::PGList *list) {
  for (auto c = list->head; c != nullptr; c = lnext(c)) {
    // auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
    group_by_.emplace_back(make_unique<BoundExpression>());
  }
}

void SelectStatement::BindHaving(duckdb_libpgquery::PGNode *root) { having_ = BindExpression(root); }

auto SelectStatement::BindExpression(duckdb_libpgquery::PGNode *node) -> unique_ptr<BoundExpression> {
  switch (node->type) {
    case duckdb_libpgquery::T_PGColumnRef:
      return BindColumnRef(reinterpret_cast<duckdb_libpgquery::PGColumnRef *>(node));
    case duckdb_libpgquery::T_PGAConst:
      return BindConstant(reinterpret_cast<duckdb_libpgquery::PGAConst *>(node));
    case duckdb_libpgquery::T_PGResTarget:
      return BindResTarget(reinterpret_cast<duckdb_libpgquery::PGResTarget *>(node));
    case duckdb_libpgquery::T_PGAStar:
      return BindStar(reinterpret_cast<duckdb_libpgquery::PGAStar *>(node));
    case duckdb_libpgquery::T_PGFuncCall:
      return BindFuncCall(reinterpret_cast<duckdb_libpgquery::PGFuncCall *>(node));
    case duckdb_libpgquery::T_PGAExpr:
      return make_unique<BoundExpression>();
    default:
      throw Exception(fmt::format("Expr of type {} not implemented", Parser::NodetypeToString(node->type)));
  }
}

}  // namespace bustub
