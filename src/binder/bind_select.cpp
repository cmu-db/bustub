#include <algorithm>
#include <iterator>
#include <memory>
#include <optional>
#include <vector>
#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_statement.h"
#include "binder/bound_table_ref.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_alias.h"
#include "binder/expressions/bound_binary_op.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/expressions/bound_star.h"
#include "binder/expressions/bound_unary_op.h"
#include "binder/statement/explain_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "binder/table_ref/bound_cross_product_ref.h"
#include "binder/table_ref/bound_cte_ref.h"
#include "binder/table_ref/bound_expression_list_ref.h"
#include "binder/table_ref/bound_join_ref.h"
#include "binder/table_ref/bound_subquery_ref.h"
#include "binder/tokens.h"
#include "catalog/catalog.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "nodes/nodes.hpp"
#include "nodes/parsenodes.hpp"
#include "nodes/primnodes.hpp"
#include "pg_definitions.hpp"
#include "postgres_parser.hpp"
#include "type/type_id.h"
#include "type/value_factory.h"

namespace bustub {

auto Binder::BindValuesList(duckdb_libpgquery::PGList *list) -> std::unique_ptr<BoundExpressionListRef> {
  std::vector<std::vector<std::unique_ptr<BoundExpression>>> all_values;

  for (auto value_list = list->head; value_list != nullptr; value_list = value_list->next) {
    auto target = static_cast<duckdb_libpgquery::PGList *>(value_list->data.ptr_value);

    auto values = BindExpressionList(target);

    if (!all_values.empty()) {
      if (all_values[0].size() != values.size()) {
        throw bustub::Exception("values must have the same length");
      }
    }
    all_values.push_back(std::move(values));
  }

  if (all_values.empty()) {
    throw bustub::Exception("at least one row of values should be provided");
  }

  return std::make_unique<BoundExpressionListRef>(std::move(all_values), "<unnamed>");
}

auto Binder::BindSubquery(duckdb_libpgquery::PGSelectStmt *node, const std::string &alias)
    -> std::unique_ptr<BoundSubqueryRef> {
  std::vector<std::vector<std::string>> select_list_name;
  auto subquery = BindSelect(node);
  for (const auto &col : subquery->select_list_) {
    switch (col->type_) {
      case ExpressionType::COLUMN_REF: {
        const auto &column_ref_expr = dynamic_cast<const BoundColumnRef &>(*col);
        select_list_name.push_back(column_ref_expr.col_name_);
        continue;
      }
      case ExpressionType::ALIAS: {
        const auto &alias_expr = dynamic_cast<const BoundAlias &>(*col);
        select_list_name.push_back(std::vector{alias_expr.alias_});
        continue;
      }
      default:
        select_list_name.push_back(std::vector{fmt::format("__item#{}", universal_id_++)});
        continue;
    }
  }
  return std::make_unique<BoundSubqueryRef>(std::move(subquery), std::move(select_list_name), alias);
}

auto Binder::BindRangeSubselect(duckdb_libpgquery::PGRangeSubselect *root) -> std::unique_ptr<BoundTableRef> {
  if (root->lateral) {
    throw NotImplementedException("LATERAL in subquery is not supported");
  }

  if (root->alias != nullptr) {
    return BindSubquery(reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(root->subquery),
                        std::string(root->alias->aliasname));
  }
  return BindSubquery(reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(root->subquery),
                      fmt::format("__subquery#{}", universal_id_++));
}

auto Binder::BindCTE(duckdb_libpgquery::PGWithClause *node) -> std::vector<std::unique_ptr<BoundSubqueryRef>> {
  std::vector<std::unique_ptr<BoundSubqueryRef>> ctes;
  for (auto cte_ele = node->ctes->head; cte_ele != nullptr; cte_ele = cte_ele->next) {
    auto cte = reinterpret_cast<duckdb_libpgquery::PGCommonTableExpr *>(cte_ele->data.ptr_value);

    if (cte->ctequery == nullptr || cte->ctequery->type != duckdb_libpgquery::T_PGSelectStmt) {
      throw bustub::Exception("SELECT not found");
    }

    if (cte->cterecursive || node->recursive) {
      throw bustub::NotImplementedException("recursive CTE not supported");
    }

    auto subquery = BindSubquery(reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(cte->ctequery), cte->ctename);

    ctes.emplace_back(std::move(subquery));
  }

  return ctes;
}

auto Binder::BindSelect(duckdb_libpgquery::PGSelectStmt *pg_stmt) -> std::unique_ptr<SelectStatement> {
  auto ctx_guard = NewContext();
  // Bind VALUES clause.
  if (pg_stmt->valuesLists != nullptr) {
    auto values_list_name = fmt::format("__values#{}", universal_id_++);
    auto value_list = BindValuesList(pg_stmt->valuesLists);
    value_list->identifier_ = values_list_name;
    std::vector<std::unique_ptr<BoundExpression>> exprs;
    size_t expr_length = value_list->values_[0].size();
    for (size_t i = 0; i < expr_length; i++) {
      exprs.emplace_back(std::make_unique<BoundColumnRef>(std::vector{values_list_name, fmt::format("{}", i)}));
    }
    return std::make_unique<SelectStatement>(
        std::move(value_list), std::move(exprs), std::make_unique<BoundExpression>(),
        std::vector<std::unique_ptr<BoundExpression>>{}, std::make_unique<BoundExpression>(),
        std::make_unique<BoundExpression>(), std::make_unique<BoundExpression>(),
        std::vector<std::unique_ptr<BoundOrderBy>>{}, std::vector<std::unique_ptr<BoundSubqueryRef>>{}, false);
  }

  // Bind CTEs
  auto ctes = std::vector<std::unique_ptr<BoundSubqueryRef>>{};
  if (pg_stmt->withClause != nullptr) {
    ctes = BindCTE(pg_stmt->withClause);
    cte_scope_ = &ctes;
    // TODO(chi): allow access CTE from multiple levels of scopes
  }

  // Bind FROM clause.
  auto table = BindFrom(pg_stmt->fromClause);
  scope_ = table.get();

  // Bind DISTINCT.
  bool is_distinct = false;
  if (pg_stmt->distinctClause != nullptr) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGNode *>(pg_stmt->distinctClause->head->data.ptr_value);
    if (target != nullptr) {
      throw NotImplementedException("DISTINCT ON is not supported");
    }
    is_distinct = true;
  }

  // Bind SELECT list.
  if (pg_stmt->targetList == nullptr) {
    throw bustub::Exception("no select list");
  }

  auto select_list = BindSelectList(pg_stmt->targetList);

  // Bind WHERE clause.
  auto where = std::make_unique<BoundExpression>();
  if (pg_stmt->whereClause != nullptr) {
    where = BindWhere(pg_stmt->whereClause);
  }

  // Bind GROUP BY clause.
  auto group_by = std::vector<std::unique_ptr<BoundExpression>>{};
  if (pg_stmt->groupClause != nullptr) {
    group_by = BindGroupBy(pg_stmt->groupClause);
  }

  // Bind HAVING clause.
  auto having = std::make_unique<BoundExpression>();
  if (pg_stmt->havingClause != nullptr) {
    having = BindHaving(pg_stmt->havingClause);
  }

  auto limit_count = std::make_unique<BoundExpression>();
  // Bind LIMIT clause.
  if (pg_stmt->limitCount != nullptr) {
    limit_count = BindLimitCount(pg_stmt->limitCount);
  }

  // Bind OFFSET clause.
  auto limit_offset = std::make_unique<BoundExpression>();
  if (pg_stmt->limitOffset != nullptr) {
    limit_offset = BindLimitOffset(pg_stmt->limitOffset);
  }

  // Bind ORDER BY clause.
  auto sort = std::vector<std::unique_ptr<BoundOrderBy>>{};
  if (pg_stmt->sortClause != nullptr) {
    sort = BindSort(pg_stmt->sortClause);
  }

  // TODO(chi): If there are any extra args (e.g. group by, having) not supported by the binder,
  // we should have thrown an exception. However, this is too tedious to implement (need to check
  // every field manually). Therefore, I'd prefer warning users that the binder is not complete
  // in project write-ups / READMEs.

  return std::make_unique<SelectStatement>(std::move(table), std::move(select_list), std::move(where),
                                           std::move(group_by), std::move(having), std::move(limit_count),
                                           std::move(limit_offset), std::move(sort), std::move(ctes), is_distinct);
}

auto Binder::BindFrom(duckdb_libpgquery::PGList *list) -> std::unique_ptr<BoundTableRef> {
  auto ctx_guard = NewContext();

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

auto Binder::BindJoin(duckdb_libpgquery::PGJoinExpr *root) -> std::unique_ptr<BoundTableRef> {
  auto ctx_guard = NewContext();
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
  scope_ = join_ref.get();
  auto condition = BindExpression(root->quals);
  join_ref->condition_ = std::move(condition);
  return join_ref;
}

auto Binder::BindBaseTableRef(std::string table_name, std::optional<std::string> alias)
    -> std::unique_ptr<BoundBaseTableRef> {
  auto table_info = catalog_.GetTable(table_name);
  if (table_info == nullptr) {
    throw bustub::Exception(fmt::format("invalid table {}", table_name));
  }
  return std::make_unique<BoundBaseTableRef>(std::move(table_name), table_info->oid_, std::move(alias),
                                             table_info->schema_);
}

auto Binder::BindRangeVar(duckdb_libpgquery::PGRangeVar *table_ref) -> std::unique_ptr<BoundTableRef> {
  if (cte_scope_ != nullptr) {
    // Firstly, find the table in CTE list.
    for (const auto &cte : *cte_scope_) {
      if (cte->alias_ == table_ref->relname) {
        std::string bound_name;
        if (table_ref->alias != nullptr) {
          bound_name = table_ref->alias->aliasname;
        } else {
          bound_name = table_ref->relname;
        }
        return std::make_unique<BoundCTERef>(cte->alias_, std::move(bound_name));
      }
    }
  }
  if (table_ref->alias != nullptr) {
    return BindBaseTableRef(table_ref->relname, std::make_optional(table_ref->alias->aliasname));
  }
  return BindBaseTableRef(table_ref->relname, std::nullopt);
}

auto Binder::BindTableRef(duckdb_libpgquery::PGNode *node) -> std::unique_ptr<BoundTableRef> {
  switch (node->type) {
    case duckdb_libpgquery::T_PGRangeVar: {
      return BindRangeVar(reinterpret_cast<duckdb_libpgquery::PGRangeVar *>(node));
    }
    case duckdb_libpgquery::T_PGJoinExpr: {
      return BindJoin(reinterpret_cast<duckdb_libpgquery::PGJoinExpr *>(node));
    }
    case duckdb_libpgquery::T_PGRangeSubselect: {
      return BindRangeSubselect(reinterpret_cast<duckdb_libpgquery::PGRangeSubselect *>(node));
    }
    default:
      throw bustub::Exception(fmt::format("unsupported node type: {}", Binder::NodeTagToString(node->type)));
  }
}

auto Binder::GetAllColumns(const BoundTableRef &scope) -> std::vector<std::unique_ptr<BoundExpression>> {
  switch (scope.type_) {
    case TableReferenceType::BASE_TABLE: {
      const auto &base_table_ref = dynamic_cast<const BoundBaseTableRef &>(scope);
      auto bound_table_name = base_table_ref.GetBoundTableName();
      const auto &schema = base_table_ref.schema_;
      auto columns = std::vector<std::unique_ptr<BoundExpression>>{};
      for (const auto &column : schema.GetColumns()) {
        columns.push_back(std::make_unique<BoundColumnRef>(std::vector{bound_table_name, column.GetName()}));
      }
      return columns;
    }
    case TableReferenceType::CROSS_PRODUCT: {
      const auto &cross_product_ref = dynamic_cast<const BoundCrossProductRef &>(scope);
      auto columns = GetAllColumns(*cross_product_ref.left_);
      auto append_columns = GetAllColumns(*cross_product_ref.right_);
      std::copy(std::make_move_iterator(append_columns.begin()), std::make_move_iterator(append_columns.end()),
                std::back_inserter(columns));
      return columns;
    }
    case TableReferenceType::JOIN: {
      const auto &join_ref = dynamic_cast<const BoundJoinRef &>(scope);
      auto columns = GetAllColumns(*join_ref.left_);
      auto append_columns = GetAllColumns(*join_ref.right_);
      std::copy(std::make_move_iterator(append_columns.begin()), std::make_move_iterator(append_columns.end()),
                std::back_inserter(columns));
      return columns;
    }
    case TableReferenceType::SUBQUERY: {
      const auto &subquery_ref = dynamic_cast<const BoundSubqueryRef &>(scope);
      auto columns = std::vector<std::unique_ptr<BoundExpression>>{};
      for (const auto &col_name : subquery_ref.select_list_name_) {
        columns.emplace_back(BoundColumnRef::Prepend(std::make_unique<BoundColumnRef>(col_name), subquery_ref.alias_));
      }
      return columns;
    }
    case TableReferenceType::CTE: {
      const auto &cte_ref = dynamic_cast<const BoundCTERef &>(scope);
      for (const auto &cte : *cte_scope_) {
        if (cte_ref.cte_name_ == cte->alias_) {
          // TODO(chi): disallow multiple CTE of the same alias
          auto columns = GetAllColumns(*cte);
          for (auto &column : columns) {
            auto &column_ref = dynamic_cast<BoundColumnRef &>(*column);
            column_ref.col_name_[0] = cte_ref.alias_;
          }
          return columns;
        }
      }
      UNREACHABLE("CTE not found");
    }
    default:
      throw bustub::Exception("select * cannot be used with this TableReferenceType");
  }
}

auto Binder::BindSelectList(duckdb_libpgquery::PGList *list) -> std::vector<std::unique_ptr<BoundExpression>> {
  std::vector<std::unique_ptr<BoundExpression>> exprs;
  auto select_list = std::vector<std::unique_ptr<BoundExpression>>{};
  bool is_select_star = false;

  for (auto node = list->head; node != nullptr; node = lnext(node)) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value);

    auto expr = BindExpression(target);

    if (expr->type_ == ExpressionType::STAR) {
      // Process `SELECT *`.
      if (!select_list.empty()) {
        throw bustub::Exception("select * cannot have other expressions in list");
      }
      select_list = GetAllColumns(*scope_);
      is_select_star = true;
    } else {
      if (is_select_star) {
        throw bustub::Exception("select * cannot have other expressions in list");
      }
      select_list.push_back(std::move(expr));
    }
  }

  return select_list;
}

auto Binder::BindExpressionList(duckdb_libpgquery::PGList *list) -> std::vector<std::unique_ptr<BoundExpression>> {
  std::vector<std::unique_ptr<BoundExpression>> exprs;
  auto select_list = std::vector<std::unique_ptr<BoundExpression>>{};

  for (auto node = list->head; node != nullptr; node = lnext(node)) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value);

    auto expr = BindExpression(target);

    if (expr->type_ == ExpressionType::STAR) {
      throw bustub::Exception("unsupport * in expression list");
    }

    select_list.push_back(std::move(expr));
  }

  return select_list;
}

auto Binder::BindConstant(duckdb_libpgquery::PGAConst *node) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(node, "nullptr");
  const auto &val = node->val;
  switch (val.type) {
    case duckdb_libpgquery::T_PGInteger: {
      BUSTUB_ENSURE(val.val.ival <= BUSTUB_INT32_MAX, "value out of range");
      return std::make_unique<BoundConstant>(ValueFactory::GetIntegerValue(static_cast<int32_t>(val.val.ival)));
    }
    case duckdb_libpgquery::T_PGString: {
      return std::make_unique<BoundConstant>(ValueFactory::GetVarcharValue(val.val.str));
    }
    case duckdb_libpgquery::T_PGNull: {
      // TODO(chi): cast integer null to other types
      return std::make_unique<BoundConstant>(ValueFactory::GetNullValueByType(TypeId::INTEGER));
    }
    default:
      break;
  }
  throw bustub::Exception(fmt::format("unsupported pg value: {}", Binder::NodeTagToString(val.type)));
}

auto Binder::BindColumnRef(duckdb_libpgquery::PGColumnRef *node) -> std::unique_ptr<BoundExpression> {
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
      return ResolveColumn(*scope_, column_names);
    }
    case duckdb_libpgquery::T_PGAStar: {
      return BindStar(reinterpret_cast<duckdb_libpgquery::PGAStar *>(head_node));
    }
    default:
      throw bustub::Exception(
          fmt::format("ColumnRef type {} not implemented!", Binder::NodeTagToString(head_node->type)));
  }
}

auto Binder::BindResTarget(duckdb_libpgquery::PGResTarget *root) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(root, "nullptr");
  auto expr = BindExpression(root->val);
  if (!expr) {
    return nullptr;
  }
  if (root->name != nullptr) {
    return std::make_unique<BoundAlias>(root->name, std::move(expr));
  }
  return expr;
}

auto Binder::BindStar(duckdb_libpgquery::PGAStar *node) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(node, "nullptr");
  return std::make_unique<BoundStar>();
}

auto Binder::BindFuncCall(duckdb_libpgquery::PGFuncCall *root) -> std::unique_ptr<BoundExpression> {
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
    return std::make_unique<BoundAggCall>(function_name, root->agg_distinct, std::move(children));
  }
  throw bustub::Exception(fmt::format("unsupported func call {}", function_name));
}

/**
 * @brief Get `BoundColumnRef` from the schema.
 */
static auto ResolveColumnRefFromSchema(const Schema &schema, const std::vector<std::string> &col_name)
    -> std::unique_ptr<BoundColumnRef> {
  if (col_name.size() != 1) {
    return nullptr;
  }
  std::unique_ptr<BoundColumnRef> column_ref = nullptr;
  for (const auto &column : schema.GetColumns()) {
    if (StringUtil::Lower(column.GetName()) == col_name[0]) {
      if (column_ref != nullptr) {
        throw Exception(fmt::format("{} is ambiguous in schema", fmt::join(col_name, ".")));
      }
      column_ref = std::make_unique<BoundColumnRef>(std::vector{column.GetName()});
    }
  }
  return column_ref;
}

/**
 * @brief Get `BoundColumnRef` from the table. Returns something like `alias.column` or `table_name.column`.
 */
auto Binder::ResolveColumnRefFromBaseTableRef(const BoundBaseTableRef &table_ref,
                                              const std::vector<std::string> &col_name)
    -> std::unique_ptr<BoundColumnRef> {
  auto bound_table_name = table_ref.GetBoundTableName();
  // Firstly, try directly resolve the column name through schema
  std::unique_ptr<BoundColumnRef> direct_resolved_expr =
      BoundColumnRef::Prepend(ResolveColumnRefFromSchema(table_ref.schema_, col_name), bound_table_name);

  std::unique_ptr<BoundColumnRef> strip_resolved_expr = nullptr;

  // Then, try strip the prefix and match again
  if (col_name.size() > 1) {
    // Strip alias and resolve again
    if (col_name[0] == bound_table_name) {
      auto strip_column_name = col_name;
      strip_column_name.erase(strip_column_name.begin());
      auto x = ResolveColumnRefFromSchema(table_ref.schema_, strip_column_name);
      strip_resolved_expr = BoundColumnRef::Prepend(std::move(x), bound_table_name);
    }
  }

  if (strip_resolved_expr != nullptr && direct_resolved_expr != nullptr) {
    throw bustub::Exception(fmt::format("{} is ambiguous in table {}", fmt::join(col_name, "."), table_ref.table_));
  }
  if (strip_resolved_expr != nullptr) {
    return strip_resolved_expr;
  }
  return direct_resolved_expr;
}

static auto MatchSuffix(const std::vector<std::string> &suffix, const std::vector<std::string> &full_name) -> bool {
  std::vector<std::string> lowercase_full_name;
  lowercase_full_name.reserve(full_name.size());
  for (const auto &col : full_name) {
    lowercase_full_name.push_back(StringUtil::Lower(col));
  }
  if (suffix.size() > lowercase_full_name.size()) {
    return false;
  }
  return std::equal(suffix.rbegin(), suffix.rend(), lowercase_full_name.rbegin());
}

auto Binder::ResolveColumnRefFromSelectList(const std::vector<std::vector<std::string>> &subquery_select_list,
                                            const std::vector<std::string> &col_name)
    -> std::unique_ptr<BoundColumnRef> {
  std::unique_ptr<BoundColumnRef> column_ref = nullptr;
  for (const auto &column_full_name : subquery_select_list) {
    if (MatchSuffix(col_name, column_full_name)) {
      if (column_ref != nullptr) {
        throw Exception(fmt::format("{} is ambiguous in subquery select list", fmt::join(col_name, ".")));
      }
      column_ref = std::make_unique<BoundColumnRef>(column_full_name);
    }
  }
  return column_ref;
}

auto Binder::ResolveColumnRefFromSubqueryRef(const BoundSubqueryRef &subquery_ref, const std::string &alias,
                                             const std::vector<std::string> &col_name)
    -> std::unique_ptr<BoundColumnRef> {
  // Firstly, try directly resolve the column name through schema
  std::unique_ptr<BoundColumnRef> direct_resolved_expr = BoundColumnRef::Prepend(
      ResolveColumnRefFromSelectList(subquery_ref.select_list_name_, col_name), subquery_ref.alias_);

  std::unique_ptr<BoundColumnRef> strip_resolved_expr = nullptr;

  // Then, try strip the prefix and match again
  if (col_name.size() > 1) {
    if (col_name[0] == alias) {
      auto strip_column_name = col_name;
      strip_column_name.erase(strip_column_name.begin());
      strip_resolved_expr = BoundColumnRef::Prepend(
          ResolveColumnRefFromSelectList(subquery_ref.select_list_name_, strip_column_name), subquery_ref.alias_);
    }
  }

  if (strip_resolved_expr != nullptr && direct_resolved_expr != nullptr) {
    throw bustub::Exception(
        fmt::format("{} is ambiguous in subquery {}", fmt::join(col_name, "."), subquery_ref.alias_));
  }
  if (strip_resolved_expr != nullptr) {
    return strip_resolved_expr;
  }
  return direct_resolved_expr;
}

auto Binder::ResolveColumnInternal(const BoundTableRef &table_ref, const std::vector<std::string> &col_name)
    -> std::unique_ptr<BoundExpression> {
  switch (table_ref.type_) {
    case TableReferenceType::BASE_TABLE: {
      const auto &base_table_ref = dynamic_cast<const BoundBaseTableRef &>(table_ref);
      return ResolveColumnRefFromBaseTableRef(base_table_ref, col_name);
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
    case TableReferenceType::SUBQUERY: {
      const auto &subquery_ref = dynamic_cast<const BoundSubqueryRef &>(table_ref);
      return ResolveColumnRefFromSubqueryRef(subquery_ref, subquery_ref.alias_, col_name);
    }
    case TableReferenceType::CTE: {
      const auto &cte_ref = dynamic_cast<const BoundCTERef &>(table_ref);
      for (const auto &cte : *cte_scope_) {
        if (cte_ref.cte_name_ == cte->alias_) {
          // TODO(chi): disallow multiple CTE of the same alias
          return ResolveColumnRefFromSubqueryRef(*cte, cte_ref.alias_, col_name);
        }
      }
      UNREACHABLE("CTE not found");
    }
    default:
      throw bustub::Exception("unsupported TableReferenceType");
  }
}

auto Binder::ResolveColumn(const BoundTableRef &scope, const std::vector<std::string> &col_name)
    -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(!scope.IsInvalid(), "invalid scope");
  auto expr = ResolveColumnInternal(scope, col_name);
  if (!expr) {
    throw bustub::Exception(fmt::format("column {} not found", fmt::join(col_name, ".")));
  }
  return expr;
}

auto Binder::BindWhere(duckdb_libpgquery::PGNode *root) -> std::unique_ptr<BoundExpression> {
  return BindExpression(root);
}

auto Binder::BindGroupBy(duckdb_libpgquery::PGList *list) -> std::vector<std::unique_ptr<BoundExpression>> {
  return BindExpressionList(list);
}

auto Binder::BindHaving(duckdb_libpgquery::PGNode *root) -> std::unique_ptr<BoundExpression> {
  return BindExpression(root);
}

auto Binder::BindAExpr(duckdb_libpgquery::PGAExpr *root) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(root, "nullptr");
  auto name = std::string((reinterpret_cast<duckdb_libpgquery::PGValue *>(root->name->head->data.ptr_value))->val.str);

  if (root->kind != duckdb_libpgquery::PG_AEXPR_OP) {
    throw bustub::Exception("unsupported op in AExpr");
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
    return std::make_unique<BoundBinaryOp>(name, std::move(left_expr), std::move(right_expr));
  }
  if (!left_expr && right_expr) {
    return std::make_unique<BoundUnaryOp>(name, std::move(right_expr));
  }
  throw bustub::Exception("unsupported AExpr: left == null while right != null");
}

auto Binder::BindBoolExpr(duckdb_libpgquery::PGBoolExpr *root) -> std::unique_ptr<BoundExpression> {
  BUSTUB_ASSERT(root, "nullptr");
  switch (root->boolop) {
    case duckdb_libpgquery::PG_AND_EXPR:
    case duckdb_libpgquery::PG_OR_EXPR: {
      std::string op_name;
      if (root->boolop == duckdb_libpgquery::PG_AND_EXPR) {
        op_name = "and";
      } else if (root->boolop == duckdb_libpgquery::PG_OR_EXPR) {
        op_name = "or";
      } else {
        UNREACHABLE("invalid op");
      }

      auto exprs = BindExpressionList(root->args);
      if (exprs.size() <= 1) {
        if (root->boolop == duckdb_libpgquery::PG_AND_EXPR) {
          throw bustub::Exception("AND should have at least 1 arg");
        }
        throw bustub::Exception("OR should have at least 1 arg");
      }
      auto expr = std::make_unique<BoundBinaryOp>(op_name, std::move(exprs[0]), std::move(exprs[1]));
      for (size_t i = 2; i < exprs.size(); i++) {
        expr = std::make_unique<BoundBinaryOp>(op_name, std::move(expr), std::move(exprs[i]));
      }
      return expr;
    }
    case duckdb_libpgquery::PG_NOT_EXPR: {
      auto exprs = BindExpressionList(root->args);
      if (exprs.size() != 1) {
        throw bustub::Exception("NOT should have 1 arg");
      }
      return std::make_unique<BoundUnaryOp>("not", std::move(exprs[0]));
    }
  }
  UNREACHABLE("We should have handled all cases!");
}

auto Binder::BindExpression(duckdb_libpgquery::PGNode *node) -> std::unique_ptr<BoundExpression> {
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
    case duckdb_libpgquery::T_PGBoolExpr:
      return BindBoolExpr(reinterpret_cast<duckdb_libpgquery::PGBoolExpr *>(node));
    default:
      break;
  }
  throw bustub::Exception(fmt::format("Expr of type {} not implemented", Binder::NodeTagToString(node->type)));
}

auto Binder::BindLimitCount(duckdb_libpgquery::PGNode *root) -> std::unique_ptr<BoundExpression> {
  return BindExpression(root);
}

auto Binder::BindLimitOffset(duckdb_libpgquery::PGNode *root) -> std::unique_ptr<BoundExpression> {
  return BindExpression(root);
}

auto Binder::BindExplain(duckdb_libpgquery::PGExplainStmt *stmt) -> std::unique_ptr<ExplainStatement> {
  uint8_t explain_options =
      ExplainOptions::PLANNER | ExplainOptions::OPTIMIZER | ExplainOptions::BINDER | ExplainOptions::SCHEMA;
  if (stmt->options != nullptr) {
    explain_options = ExplainOptions::INVALID;
    for (auto node = stmt->options->head; node != nullptr; node = node->next) {
      auto temp = reinterpret_cast<duckdb_libpgquery::PGDefElem *>(node->data.ptr_value);
      if (strcmp(temp->defname, "planner") == 0 || strcmp(temp->defname, "p") == 0) {
        explain_options |= ExplainOptions::PLANNER;
      }
      if (strcmp(temp->defname, "binder") == 0 || strcmp(temp->defname, "b") == 0) {
        explain_options |= ExplainOptions::BINDER;
      }
      if (strcmp(temp->defname, "optimizer") == 0 || strcmp(temp->defname, "o") == 0) {
        explain_options |= ExplainOptions::OPTIMIZER;
      }
      if (strcmp(temp->defname, "schema") == 0 || strcmp(temp->defname, "s") == 0) {
        explain_options |= ExplainOptions::SCHEMA;
      }
    }
  }
  return std::make_unique<ExplainStatement>(BindStatement(stmt->query), explain_options);
}

//===----------------------------------------------------------------------===//
// Copyright 2018-2022 Stichting DuckDB Foundation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice (including the next paragraph)
// shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//===----------------------------------------------------------------------===//

auto Binder::BindSort(duckdb_libpgquery::PGList *list) -> std::vector<std::unique_ptr<BoundOrderBy>> {
  auto order_by = std::vector<std::unique_ptr<BoundOrderBy>>{};

  for (auto node = list->head; node != nullptr; node = node->next) {
    auto temp = reinterpret_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value);
    if (temp->type == duckdb_libpgquery::T_PGSortBy) {
      OrderByType type;
      auto sort = reinterpret_cast<duckdb_libpgquery::PGSortBy *>(temp);
      auto target = sort->node;
      if (sort->sortby_dir == duckdb_libpgquery::PG_SORTBY_DEFAULT) {
        type = OrderByType::DEFAULT;
      } else if (sort->sortby_dir == duckdb_libpgquery::PG_SORTBY_ASC) {
        type = OrderByType::ASC;
      } else if (sort->sortby_dir == duckdb_libpgquery::PG_SORTBY_DESC) {
        type = OrderByType::DESC;
      } else {
        throw NotImplementedException("unimplemented order by type");
      }
      auto order_expression = BindExpression(target);
      order_by.emplace_back(std::make_unique<BoundOrderBy>(type, std::move(order_expression)));
    } else {
      throw NotImplementedException("unsupported order by node");
    }
  }
  return order_by;
}

//===----------------------------------------------------------------------===//
// End Copyright 2018-2022 Stichting DuckDB Foundation
//===----------------------------------------------------------------------===//

}  // namespace bustub
