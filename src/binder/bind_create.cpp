//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bind_create.cpp
//
// Identification: src/binder/bind_create.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

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

#include <iterator>
#include <memory>
#include <string>
#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_agg_call.h"
#include "binder/expressions/bound_binary_op.h"
#include "binder/expressions/bound_column_ref.h"
#include "binder/expressions/bound_constant.h"
#include "binder/expressions/bound_star.h"
#include "binder/expressions/bound_unary_op.h"
#include "binder/statement/create_statement.h"
#include "binder/statement/index_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "binder/table_ref/bound_cross_product_ref.h"
#include "binder/table_ref/bound_join_ref.h"
#include "binder/tokens.h"
#include "catalog/catalog.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "nodes/nodes.hpp"
#include "nodes/primnodes.hpp"
#include "nodes/value.hpp"
#include "pg_definitions.hpp"
#include "postgres_parser.hpp"
#include "type/type_id.h"

namespace bustub {

auto Binder::BindColumnDefinition(duckdb_libpgquery::PGColumnDef *cdef) -> Column {
  std::string colname;
  if (cdef->colname != nullptr) {
    colname = cdef->colname;
  }
  if (cdef->collClause != nullptr) {
    throw NotImplementedException("coll clause on column is not supported");
  }

  auto name = std::string(
      (reinterpret_cast<duckdb_libpgquery::PGValue *>(cdef->typeName->names->tail->data.ptr_value)->val.str));

  if (name == "int4") {
    return {colname, TypeId::INTEGER};
  }

  if (name == "double") {
    return {colname, TypeId::DECIMAL};
  }

  if (name == "bool") {
    return {colname, TypeId::BOOLEAN};
  }

  if (name == "varchar") {
    auto exprs = BindExpressionList(cdef->typeName->typmods);
    if (exprs.size() != 1) {
      throw bustub::Exception("should specify max length for varchar field");
    }
    const auto &varchar_max_length_val = dynamic_cast<const BoundConstant &>(*exprs[0]);
    uint32_t varchar_max_length = std::stoi(varchar_max_length_val.ToString());
    return {colname, TypeId::VARCHAR, varchar_max_length};
  }

  if (name == "vector") {
    auto exprs = BindExpressionList(cdef->typeName->typmods);
    if (exprs.size() != 1) {
      throw bustub::Exception("should specify vector length");
    }
    const auto &vector_length_val = dynamic_cast<const BoundConstant &>(*exprs[0]);
    uint32_t vector_length = std::stoi(vector_length_val.ToString());
    return {colname, TypeId::VECTOR, vector_length};
  }

  throw NotImplementedException(fmt::format("unsupported type: {}", name));
}

auto Binder::BindCreate(duckdb_libpgquery::PGCreateStmt *pg_stmt) -> std::unique_ptr<CreateStatement> {
  auto table = std::string(pg_stmt->relation->relname);
  auto columns = std::vector<Column>{};
  size_t column_count = 0;
  std::vector<std::string> pk;

  for (auto c = pg_stmt->tableElts->head; c != nullptr; c = lnext(c)) {
    auto node = reinterpret_cast<duckdb_libpgquery::PGNode *>(c->data.ptr_value);
    switch (node->type) {
      case duckdb_libpgquery::T_PGColumnDef: {
        auto cdef = reinterpret_cast<duckdb_libpgquery::PGColumnDef *>(c->data.ptr_value);
        auto centry = BindColumnDefinition(cdef);
        if (cdef->constraints != nullptr) {
          for (auto constr = cdef->constraints->head; constr != nullptr; constr = constr->next) {
            auto constraint = reinterpret_cast<duckdb_libpgquery::PGConstraint *>(constr->data.ptr_value);
            switch (constraint->contype) {
              case duckdb_libpgquery::PG_CONSTR_PRIMARY: {
                if (!pk.empty()) {
                  throw NotImplementedException("cannot have two primary keys");
                }
                pk = {centry.GetName()};
                break;
              }
              default:
                throw NotImplementedException("unsupported constraint");
            }
          }
        }
        columns.push_back(std::move(centry));
        column_count++;
        break;
      }
      case duckdb_libpgquery::T_PGConstraint: {
        for (auto con = c; con != nullptr; con = con->next) {
          auto constraint = reinterpret_cast<duckdb_libpgquery::PGConstraint *>(con->data.ptr_value);
          switch (constraint->contype) {
            case duckdb_libpgquery::PG_CONSTR_PRIMARY: {
              std::vector<std::string> columns;
              for (auto kc = constraint->keys->head; kc != nullptr; kc = kc->next) {
                columns.emplace_back(reinterpret_cast<duckdb_libpgquery::PGValue *>(kc->data.ptr_value)->val.str);
              }
              if (!pk.empty()) {
                throw NotImplementedException("cannot have two primary keys");
              }
              pk = std::move(columns);
              break;
            }
            default:
              throw NotImplementedException("unsupported constraint");
          }
        }
        break;
      }
      default:
        throw NotImplementedException("ColumnDef type not handled yet");
    }
  }

  if (column_count == 0) {
    throw bustub::Exception("should have at least 1 column");
  }

  return std::make_unique<CreateStatement>(std::move(table), std::move(columns), std::move(pk));
}

auto Binder::BindIndex(duckdb_libpgquery::PGIndexStmt *stmt) -> std::unique_ptr<IndexStatement> {
  std::vector<std::unique_ptr<BoundColumnRef>> cols;
  std::vector<std::string> col_options;
  auto table = BindBaseTableRef(stmt->relation->relname, std::nullopt);

  for (auto cell = stmt->indexParams->head; cell != nullptr; cell = cell->next) {
    auto index_element = reinterpret_cast<duckdb_libpgquery::PGIndexElem *>(cell->data.ptr_value);
    if (index_element->name != nullptr) {
      auto column_ref = ResolveColumn(*table, std::vector{std::string(index_element->name)});
      cols.emplace_back(std::make_unique<BoundColumnRef>(dynamic_cast<const BoundColumnRef &>(*column_ref)));
      std::string opt;
      if (index_element->opclass != nullptr) {
        for (auto c = index_element->opclass->head; c != nullptr; c = lnext(c)) {
          opt = reinterpret_cast<duckdb_libpgquery::PGValue *>(c->data.ptr_value)->val.str;
          break;
        }
      }
      col_options.emplace_back(opt);
    } else {
      throw NotImplementedException("create index by expr is not supported yet");
    }
  }

  std::string index_type;

  if (stmt->accessMethod != nullptr) {
    index_type = stmt->accessMethod;
    if (index_type == "art") {
      index_type = "";
    }
  }

  std::vector<std::pair<std::string, int>> options;

  if (stmt->options != nullptr) {
    for (auto c = stmt->options->head; c != nullptr; c = lnext(c)) {
      auto def_elem = reinterpret_cast<duckdb_libpgquery::PGDefElem *>(c->data.ptr_value);
      int val;
      if (def_elem->arg != nullptr) {
        val = reinterpret_cast<duckdb_libpgquery::PGValue *>(def_elem->arg)->val.ival;
      }
      options.emplace_back(def_elem->defname, val);
    }
  }

  return std::make_unique<IndexStatement>(stmt->idxname, std::move(table), std::move(cols), std::move(index_type),
                                          std::move(col_options), std::move(options));
}

}  // namespace bustub
