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
#include "binder/table_ref/bound_cross_product_ref.h"
#include "binder/table_ref/bound_join_ref.h"
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
  table_ = BindFrom(pg_stmt->fromClause);

  // Bind SELECT list.
  if (pg_stmt->targetList != nullptr) {
    BindSelectList(pg_stmt->targetList);
  } else {
    throw bustub::Exception("no select list");
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

auto SelectStatement::BindFrom(duckdb_libpgquery::PGList *list) -> std::unique_ptr<BoundTableRef> {
  if (list == nullptr) {
    return std::make_unique<BoundTableRef>(TableReferenceType::EMPTY);
  }
  if (list->length > 1) {
    // Bind cross join.

    // Extract the first node.
    auto c = list->head;
    auto lnode = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
    auto ltable = BindTableRef(lnode);
    c = lnext(c);

    // Extract the second node.
    auto rnode = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
    auto rtable = BindTableRef(rnode);
    c = lnext(c);

    auto result = std::make_unique<BoundCrossProductRef>(std::move(ltable), std::move(rtable));

    // Extract the remaining nodes.
    for (; c != nullptr; c = lnext(c)) {
      auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
      auto table = BindTableRef(node);
      result = std::make_unique<BoundCrossProductRef>(std::move(result), std::move(table));
    }

    /**
     * The result bound tree will be like:
     * ```
     *     O
     *    / \
     *   O   3
     *  / \
     * 1   2
     * ```
     */
    return result;
  }

  auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(list->head->data.ptr_value);
  return BindTableRef(node);
}

auto SelectStatement::BindJoin(duckdb_libpgquery::PGJoinExpr *root) -> std::unique_ptr<BoundTableRef> {
  JoinType join_type;
  switch (root->jointype) {
    case duckdb_libpgquery::PG_JOIN_INNER: {
      join_type = JoinType::INNER;
      break;
    }
    case duckdb_libpgquery::PG_JOIN_LEFT: {
      join_type = JoinType::LEFT;
      break;
    }
    case duckdb_libpgquery::PG_JOIN_FULL: {
      join_type = JoinType::OUTER;
      break;
    }
    case duckdb_libpgquery::PG_JOIN_RIGHT: {
      join_type = JoinType::RIGHT;
      break;
    }
    default: {
      throw bustub::Exception(fmt::format("Join type {} not supported", static_cast<int>(root->jointype)));
    }
  }
  auto left_table = BindTableRef(root->larg);
  auto right_table = BindTableRef(root->rarg);
  auto join_ref = std::make_unique<BoundJoinRef>(join_type, std::move(left_table), std::move(right_table), nullptr);
  auto condition = BindExpressionWithScope(root->quals, *join_ref);
  join_ref->condition_ = std::move(condition);
  return join_ref;
}

auto SelectStatement::BindTableRef(duckdb_libpgquery::PGNode *node) -> std::unique_ptr<BoundTableRef> {
  switch (node->type) {
    case duckdb_libpgquery::T_PGRangeVar: {
      auto *table_ref = reinterpret_cast<duckdb_libpgquery::PGRangeVar *>(node);
      auto table_info = catalog_.GetTable(table_ref->relname);
      if (table_info == nullptr) {
        throw bustub::Exception(fmt::format("invalid table {}", table_ref->relname));
      }
      return std::make_unique<BoundBaseTableRef>(table_ref->relname, table_info->schema_);
    }
    case duckdb_libpgquery::T_PGJoinExpr: {
      return BindJoin(reinterpret_cast<duckdb_libpgquery::PGJoinExpr *>(node));
    }
    default:
      throw bustub::Exception(fmt::format("unsupported node type: {}", Binder::NodeTagToString(node->type)));
  }
}

auto SelectStatement::ToString() const -> std::string {
  std::vector<std::string> columns;
  for (const auto &expr : select_list_) {
    columns.push_back(fmt::format("{}", expr));
  }
  return fmt::format("Select {{ table={}, columns=[{}], groupBy=[{}], having={}, where={} }}", table_,
                     fmt::join(columns, ", "), fmt::join(group_by_, ", "), having_, where_);
}

void SelectStatement::AddAllColumnsToList(const BoundTableRef &table_ref) {
  switch (table_ref.type_) {
    case TableReferenceType::BASE_TABLE: {
      const auto &base_table_ref = dynamic_cast<const BoundBaseTableRef &>(table_ref);
      const auto &table_name = base_table_ref.table_;
      const auto &schema = base_table_ref.schema_;
      for (const auto &column : schema.GetColumns()) {
        select_list_.push_back(std::make_unique<BoundColumnRef>(table_name, column.GetName()));
      }
      return;
    }
    case TableReferenceType::CROSS_PRODUCT: {
      const auto &cross_product_ref = dynamic_cast<const BoundCrossProductRef &>(table_ref);
      AddAllColumnsToList(*cross_product_ref.left_);
      AddAllColumnsToList(*cross_product_ref.right_);
      return;
    }
    case TableReferenceType::JOIN: {
      const auto &join_ref = dynamic_cast<const BoundJoinRef &>(table_ref);
      AddAllColumnsToList(*join_ref.left_);
      AddAllColumnsToList(*join_ref.right_);
      return;
    }
    default:
      throw bustub::Exception("select * cannot be used with this TableReferenceType");
  }
}

void SelectStatement::BindSelectList(duckdb_libpgquery::PGList *list) {
  std::vector<std::unique_ptr<BoundExpression>> exprs;

  for (auto node = list->head; node != nullptr; node = lnext(node)) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value);

    auto expr = BindExpression(target);

    // Process `SELECT *`.
    if (expr->type_ == ExpressionType::STAR) {
      if (select_list_.empty()) {
        AddAllColumnsToList(*table_);
      } else {
        throw bustub::Exception("select * cannot have other expressions in list");
      }
    } else {
      select_list_.push_back(std::move(expr));
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
      throw bustub::Exception(fmt::format("unsupported pg value: {}", Binder::NodeTagToString(val.type)));
  }
  return std::make_unique<BoundConstant>(std::move(bound_val));
}

auto SelectStatement::BindColumnRef(duckdb_libpgquery::PGColumnRef *node, const bustub::BoundTableRef &scope)
    -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(node, "nullptr");
  auto fields = node->fields;
  auto head_node = static_cast<duckdb_libpgquery::PGNode *>(fields->head->data.ptr_value);
  switch (head_node->type) {
    case duckdb_libpgquery::T_PGString: {
      if (fields->length < 1) {
        throw bustub::Exception("Unexpected field length");
      }
      std::vector<std::string> column_names;
      for (auto node = fields->head; node != nullptr; node = node->next) {
        column_names.emplace_back(reinterpret_cast<duckdb_libpgquery::PGValue *>(node->data.ptr_value)->val.str);
      }
      if (column_names.size() == 1) {
        // Bind `SELECT col`.
        return ResolveColumn(scope, column_names);
      }
      if (column_names.size() == 2) {
        // Bind `SELECT table.col`.
        return ResolveColumn(scope, column_names);
      }
      throw bustub::Exception(fmt::format("unsupported ColumnRef: zero or multiple elements found"));
    }
    case duckdb_libpgquery::T_PGAStar: {
      return BindStar(reinterpret_cast<duckdb_libpgquery::PGAStar *>(head_node));
    }
    default:
      throw bustub::Exception(
          fmt::format("ColumnRef type {} not implemented!", Binder::NodeTagToString(head_node->type)));
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
      children.push_back(std::move(child_expr));
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
  throw bustub::Exception(fmt::format("unsupported func call {}", function_name));
}

static auto ResolveColumnInternal(const BoundTableRef &table_ref, const std::vector<std::string> &col_name)
    -> std::unique_ptr<BoundExpression> {
  switch (table_ref.type_) {
    case TableReferenceType::BASE_TABLE: {
      const auto &base_table_ref = dynamic_cast<const BoundBaseTableRef &>(table_ref);
      // TODO(chi): handle case-insensitive table / column names
      const auto &table_name = base_table_ref.table_;
      auto expr = std::make_unique<BoundExpression>();
      for (const auto &column : base_table_ref.schema_.GetColumns()) {
        auto table_col_name = column.GetName();
        if (col_name.size() == 1) {
          if (StringUtil::Lower(table_col_name) == col_name[0]) {
            return std::make_unique<BoundColumnRef>(table_name, table_col_name);
          }
        } else if (col_name.size() == 2) {
          if (StringUtil::Lower(table_name) == col_name[0] && StringUtil::Lower(table_col_name) == col_name[1]) {
            return std::make_unique<BoundColumnRef>(table_name, table_col_name);
          }
        } else {
          throw Exception(fmt::format("unsupported column name: {}", fmt::join(col_name, ".")));
        }
      }
      return nullptr;
    }
    case TableReferenceType::CROSS_PRODUCT: {
      const auto &cross_product_ref = dynamic_cast<const BoundCrossProductRef &>(table_ref);
      auto left_column = ResolveColumnInternal(*cross_product_ref.left_, col_name);
      auto right_column = ResolveColumnInternal(*cross_product_ref.right_, col_name);
      if (left_column && right_column) {
        throw Exception(fmt::format("{} is ambiguous", fmt::join(col_name, ".")));
      }
      if (left_column != nullptr) {
        return left_column;
      }
      return right_column;
    }
    case TableReferenceType::JOIN: {
      const auto &join_ref = dynamic_cast<const BoundJoinRef &>(table_ref);
      auto left_column = ResolveColumnInternal(*join_ref.left_, col_name);
      auto right_column = ResolveColumnInternal(*join_ref.right_, col_name);
      if (left_column != nullptr && right_column != nullptr) {
        throw Exception(fmt::format("{} is ambiguous", fmt::join(col_name, ".")));
      }
      if (left_column != nullptr) {
        return left_column;
      }
      return right_column;
    }
    default:
      throw bustub::Exception("unsupported TableReferenceType");
  }
}

auto SelectStatement::ResolveColumn(const BoundTableRef &table_ref, const std::vector<std::string> &col_name)
    -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(!table_ref.IsInvalid(), "invalid table");
  auto expr = ResolveColumnInternal(table_ref, col_name);
  if (!expr) {
    throw bustub::Exception(fmt::format("column {} not found", fmt::join(col_name, ".")));
  }
  return expr;
}

void SelectStatement::BindWhere(duckdb_libpgquery::PGNode *root) { where_ = BindExpression(root); }

void SelectStatement::BindGroupBy(duckdb_libpgquery::PGList *list) {
  for (auto c = list->head; c != nullptr; c = lnext(c)) {
    auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
    group_by_.emplace_back(BindExpression(node));
  }
}

void SelectStatement::BindHaving(duckdb_libpgquery::PGNode *root) { having_ = BindExpression(root); }

auto SelectStatement::BindAExpr(duckdb_libpgquery::PGAExpr *root, const bustub::BoundTableRef &scope)
    -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(root, "nullptr");
  auto name = std::string((reinterpret_cast<duckdb_libpgquery::PGValue *>(root->name->head->data.ptr_value))->val.str);

  if (root->kind != duckdb_libpgquery::PG_AEXPR_OP) {
    throw bustub::Exception("unsupported op in AExpr");
  }

  std::unique_ptr<BoundExpression> left_expr = nullptr;
  std::unique_ptr<BoundExpression> right_expr = nullptr;

  if (root->lexpr != nullptr) {
    left_expr = BindExpressionWithScope(root->lexpr, scope);
  }
  if (root->rexpr != nullptr) {
    right_expr = BindExpressionWithScope(root->rexpr, scope);
  }

  if (left_expr && right_expr) {
    return std::make_unique<BoundBinaryOp>(name, std::move(left_expr), std::move(right_expr));
  }
  if (!left_expr && right_expr) {
    return std::make_unique<BoundUnaryOp>(name, std::move(right_expr));
  }
  throw bustub::Exception("unsupported AExpr: left == null while right != null");
}

auto SelectStatement::BindExpression(duckdb_libpgquery::PGNode *node) -> std::unique_ptr<BoundExpression> {
  return BindExpressionWithScope(node, *table_);
}

auto SelectStatement::BindExpressionWithScope(duckdb_libpgquery::PGNode *node, const bustub::BoundTableRef &scope)
    -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(node, "nullptr");
  switch (node->type) {
    case duckdb_libpgquery::T_PGColumnRef:
      return BindColumnRef(reinterpret_cast<duckdb_libpgquery::PGColumnRef *>(node), scope);
    case duckdb_libpgquery::T_PGAConst:
      return BindConstant(reinterpret_cast<duckdb_libpgquery::PGAConst *>(node));
    case duckdb_libpgquery::T_PGResTarget:
      return BindResTarget(reinterpret_cast<duckdb_libpgquery::PGResTarget *>(node));
    case duckdb_libpgquery::T_PGAStar:
      return BindStar(reinterpret_cast<duckdb_libpgquery::PGAStar *>(node));
    case duckdb_libpgquery::T_PGFuncCall:
      return BindFuncCall(reinterpret_cast<duckdb_libpgquery::PGFuncCall *>(node));
    case duckdb_libpgquery::T_PGAExpr:
      return BindAExpr(reinterpret_cast<duckdb_libpgquery::PGAExpr *>(node), scope);
    default:
      throw bustub::Exception(fmt::format("Expr of type {} not implemented", Binder::NodeTagToString(node->type)));
  }
}

}  // namespace bustub
