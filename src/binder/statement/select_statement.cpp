#include "binder/statement/select_statement.h"
#include "binder/binder.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_binary_op.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/expressions/bound_star.h"
#include "binder/expressions/bound_unary_op.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "binder/tokens.h"
#include "catalog/catalog.h"
#include "common/util/string_util.h"
#include "fmt/format.h"
#include "pg_definitions.hpp"
#include "postgres_parser.hpp"

namespace bustub {

SelectStatement::SelectStatement(const Catalog &catalog, duckdb_libpgquery::PGSelectStmt *pg_stmt)
    : BoundStatement(StatementType::SELECT_STATEMENT),
      table_(std::make_unique<BoundTableRef>()),
      where_(std::make_unique<BoundExpression>()),
      having_(std::make_unique<BoundExpression>()),
      catalog_(catalog) {
  // Bind FROM clause.
  if (pg_stmt->fromClause != nullptr) {
    // Extract the table name from the FROM clause.
    for (auto c = pg_stmt->fromClause->head; c != nullptr; c = lnext(c)) {
      auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
      switch (node->type) {
        case duckdb_libpgquery::T_PGRangeVar: {
          auto *table_ref = reinterpret_cast<duckdb_libpgquery::PGRangeVar *>(node);
          table_ = std::make_unique<BoundBaseTableRef>(table_ref->relname);
          break;
        }
        default:
          throw Exception(fmt::format("unsupported node type: {}", Binder::NodeTagToString(node->type)));
      }
    }
  } else {
    table_ = std::make_unique<BoundTableRef>(TableReferenceType::EMPTY);
  }

  // Bind SELECT list.
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

auto SelectStatement::ToString() const -> std::string {
  std::vector<std::string> columns;
  for (const auto &expr : select_list_) {
    columns.push_back(fmt::format("{}", expr));
  }
  return fmt::format("Select {{ table={}, columns=[{}], groupBy=[{}], having={}, where={} }}", table_,
                     fmt::join(columns, ", "), fmt::join(group_by_, ", "), having_, where_);
}

void SelectStatement::BindSelectList(duckdb_libpgquery::PGList *list) {
  std::vector<std::unique_ptr<BoundExpression>> exprs;

  for (auto node = list->head; node != nullptr; node = lnext(node)) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value);

    auto expr = BindExpression(target);

    // Process `SELECT *`.
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
              select_list_.push_back(std::make_unique<BoundColumnRef>(table_name, column.GetName()));
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

auto SelectStatement::BindConstant(duckdb_libpgquery::PGAConst *node) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(node, "nullptr");
  auto bound_val = Value(TypeId::INTEGER);
  const auto &val = node->val;
  switch (val.type) {
    case duckdb_libpgquery::T_PGInteger:
      BUSTUB_ASSERT(val.val.ival <= BUSTUB_INT32_MAX, "value out of range");
      bound_val = Value(TypeId::INTEGER, static_cast<int32_t>(val.val.ival));
      break;
    default:
      throw Exception(fmt::format("unsupported pg value: {}", Binder::NodeTagToString(val.type)));
  }
  return std::make_unique<BoundConstant>(std::move(bound_val));
}

auto SelectStatement::BindColumnRef(duckdb_libpgquery::PGColumnRef *node) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(node, "nullptr");
  auto fields = node->fields;
  auto head_node = static_cast<duckdb_libpgquery::PGNode *>(fields->head->data.ptr_value);
  switch (head_node->type) {
    case duckdb_libpgquery::T_PGString: {
      if (fields->length < 1) {
        throw Exception("Unexpected field length");
      }
      std::vector<std::string> column_names;
      for (auto node = fields->head; node != nullptr; node = node->next) {
        column_names.emplace_back(reinterpret_cast<duckdb_libpgquery::PGValue *>(node->data.ptr_value)->val.str);
      }
      if (column_names.size() == 1) {
        // Bind `SELECT col`.
        return ResolveColumn(column_names[0]);
      }
      if (column_names.size() == 2) {
        // Bind `SELECT table.col`.
        return ResolveColumnWithTable(column_names[0], column_names[1]);
      }
      throw Exception(fmt::format("unsupported ColumnRef: zero or multiple elements found"));
    }
    case duckdb_libpgquery::T_PGAStar: {
      return BindStar(reinterpret_cast<duckdb_libpgquery::PGAStar *>(head_node));
    }
    default:
      throw Exception(fmt::format("ColumnRef type {} not implemented!", Binder::NodeTagToString(head_node->type)));
  }
}

auto SelectStatement::BindResTarget(duckdb_libpgquery::PGResTarget *root) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(root, "nullptr");
  auto expr = BindExpression(root->val);
  if (!expr) {
    return nullptr;
  }
  return expr;
}

auto SelectStatement::BindStar(duckdb_libpgquery::PGAStar *node) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(node, "nullptr");
  return std::make_unique<BoundStar>();
}

auto SelectStatement::BindFuncCall(duckdb_libpgquery::PGFuncCall *root) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(root, "nullptr");
  auto name = root->funcname;
  auto function_name =
      StringUtil::Lower(reinterpret_cast<duckdb_libpgquery::PGValue *>(name->head->data.ptr_value)->val.str);

  std::vector<std::unique_ptr<BoundExpression>> children;
  if (root->args != nullptr) {
    for (auto node = root->args->head; node != nullptr; node = node->next) {
      auto child_expr = BindExpression(static_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value));
      children.push_back(move(child_expr));
    }
  }

  if (function_name == "min" || function_name == "max" || function_name == "first" || function_name == "last" ||
      function_name == "sum" || function_name == "count") {
    // Rewrite count(*) to count_star().
    if (function_name == "count" && children.empty()) {
      function_name = "count_star";
    }

    // Bind function as agg call.
    return std::make_unique<BoundAggCall>(function_name, move(children));
  }
  throw Exception(fmt::format("unsupported func call {}", function_name));
}

auto SelectStatement::ResolveColumn(const std::string &col_name) -> std::unique_ptr<BoundExpression> {
  switch (table_->type_) {
    case TableReferenceType::BASE_TABLE: {
      auto base_table_ref = dynamic_cast<BoundBaseTableRef *>(table_.get());
      // TODO(chi): handle case-insensitive table / column names
      const auto &table_name = base_table_ref->table_;
      auto table_info = catalog_.GetTable(table_name);
      if (table_info == nullptr) {
        throw Exception(fmt::format("invalid table {}", table_name));
      }
      bool found = false;
      auto expr = std::make_unique<BoundExpression>();
      for (const auto &column : table_info->schema_.GetColumns()) {
        auto name = column.GetName();
        if (StringUtil::Lower(name) == col_name) {
          if (!found) {
            expr = std::make_unique<BoundColumnRef>(table_name, name);
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

auto SelectStatement::ResolveColumnWithTable(const std::string &table_name, const std::string &col_name)
    -> std::unique_ptr<BoundExpression> {
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
  auto expr = std::make_unique<BoundExpression>();
  for (const auto &column : table_info->schema_.GetColumns()) {
    auto name = column.GetName();
    if (StringUtil::Lower(name) == col_name) {
      if (!found) {
        expr = std::make_unique<BoundColumnRef>(table_name, name);
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
    auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
    group_by_.emplace_back(BindExpression(node));
  }
}

void SelectStatement::BindHaving(duckdb_libpgquery::PGNode *root) { having_ = BindExpression(root); }

auto SelectStatement::BindAExpr(duckdb_libpgquery::PGAExpr *root) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(root, "nullptr");
  auto name = std::string((reinterpret_cast<duckdb_libpgquery::PGValue *>(root->name->head->data.ptr_value))->val.str);

  if (root->kind != duckdb_libpgquery::PG_AEXPR_OP) {
    throw Exception("unsupported op in AExpr");
  }

  std::unique_ptr<BoundExpression> left_expr = nullptr;
  std::unique_ptr<BoundExpression> right_expr = nullptr;

  if (root->lexpr != nullptr) {
    left_expr = BindExpression(root->lexpr);
  }
  if (root->rexpr != nullptr) {
    right_expr = BindExpression(root->rexpr);
  }

  if (left_expr && right_expr) {
    return std::make_unique<BoundBinaryOp>(name, move(left_expr), move(right_expr));
  }
  if (!left_expr && right_expr) {
    return std::make_unique<BoundUnaryOp>(name, move(right_expr));
  }
  throw Exception("unsupported AExpr: left == null while right != null");
}

auto SelectStatement::BindExpression(duckdb_libpgquery::PGNode *node) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(node, "nullptr");
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
      return BindAExpr(reinterpret_cast<duckdb_libpgquery::PGAExpr *>(node));
    default:
      throw Exception(fmt::format("Expr of type {} not implemented", Binder::NodeTagToString(node->type)));
  }
}

}  // namespace bustub
