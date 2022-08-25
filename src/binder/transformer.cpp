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

#include "binder/parser.h"

#include "binder/sql_statement.h"
#include "binder/statement/create_statement.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "common/exception.h"
#include "common/logger.h"
#include "common/util/string_util.h"
#include "type/decimal_type.h"

namespace bustub {

vector<unique_ptr<SQLStatement>> Parser::TransformParseTree(duckdb_libpgquery::PGList *tree) const {
  vector<unique_ptr<SQLStatement>> statements;
  for (auto entry = tree->head; entry != nullptr; entry = entry->next) {
    auto stmt = TransformStatement(static_cast<duckdb_libpgquery::PGNode *>(entry->data.ptr_value));
    statements.push_back(move(stmt));
  }
  return statements;
}

unique_ptr<SQLStatement> Parser::TransformStatement(duckdb_libpgquery::PGNode *stmt) const {
  switch (stmt->type) {
    case duckdb_libpgquery::T_PGRawStmt: {
      auto raw_stmt = reinterpret_cast<duckdb_libpgquery::PGRawStmt *>(stmt);
      auto result = TransformStatement(raw_stmt->stmt);
      if (result) {
        result->stmt_location_ = raw_stmt->stmt_location;
        result->stmt_length_ = raw_stmt->stmt_len;
      }
      return result;
    }
    case duckdb_libpgquery::T_PGCreateStmt:
      return make_unique<CreateStatement>(*this, reinterpret_cast<duckdb_libpgquery::PGCreateStmt *>(stmt));
    case duckdb_libpgquery::T_PGInsertStmt:
      return make_unique<InsertStatement>(*this, reinterpret_cast<duckdb_libpgquery::PGInsertStmt *>(stmt));
    case duckdb_libpgquery::T_PGSelectStmt:
      return std::make_unique<SelectStatement>(*this, reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(stmt));
    case duckdb_libpgquery::T_PGDeleteStmt:
      return make_unique<DeleteStatement>(*this, reinterpret_cast<duckdb_libpgquery::PGDeleteStmt *>(stmt));
    case duckdb_libpgquery::T_PGIndexStmt:
    case duckdb_libpgquery::T_PGUpdateStmt:
    default:
      throw NotImplementedException(NodetypeToString(stmt->type));
  }
}

static const std::pair<std::string, TypeId> INTERNAL_TYPES[] = {
    {"int", TypeId::INTEGER},        {"int4", TypeId::INTEGER},
    {"signed", TypeId::INTEGER},     {"integer", TypeId::INTEGER},
    {"integral", TypeId::INTEGER},   {"int32", TypeId::INTEGER},
    {"varchar", TypeId::VARCHAR},    {"bpchar", TypeId::VARCHAR},
    {"text", TypeId::VARCHAR},       {"string", TypeId::VARCHAR},
    {"char", TypeId::VARCHAR},       {"nvarchar", TypeId::VARCHAR},
    {"int8", TypeId::BIGINT},        {"bigint", TypeId::BIGINT},
    {"int64", TypeId::BIGINT},       {"long", TypeId::BIGINT},
    {"oid", TypeId::BIGINT},         {"int2", TypeId::SMALLINT},
    {"smallint", TypeId::SMALLINT},  {"short", TypeId::SMALLINT},
    {"int16", TypeId::SMALLINT},     {"timestamp", TypeId::TIMESTAMP},
    {"datetime", TypeId::TIMESTAMP}, {"timestamp_us", TypeId::TIMESTAMP},
    {"bool", TypeId::BOOLEAN},       {"boolean", TypeId::BOOLEAN},
    {"logical", TypeId::BOOLEAN},    {"decimal", TypeId::DECIMAL},
    {"dec", TypeId::DECIMAL},        {"numeric", TypeId::DECIMAL},
    {"real", TypeId::DECIMAL},       {"float4", TypeId::DECIMAL},
    {"float", TypeId::DECIMAL},      {"double", TypeId::DECIMAL},
    {"float8", TypeId::DECIMAL},     {"tinyint", TypeId::TINYINT},
    {"int1", TypeId::TINYINT},       {"", TypeId::INVALID}};

TypeId Parser::TransformTypeId(duckdb_libpgquery::PGTypeName *type_name) const {
  if (type_name->type != duckdb_libpgquery::T_PGTypeName) {
    throw Exception("Expected a type");
  }

  auto name = (reinterpret_cast<duckdb_libpgquery::PGValue *>(type_name->names->tail->data.ptr_value)->val.str);
  // transform it to the SQL type
  auto lower_str = StringUtil::Lower(name);

  for (uint64_t index = 0; !INTERNAL_TYPES[index].first.empty(); index++) {
    if (INTERNAL_TYPES[index].first == lower_str) {
      return INTERNAL_TYPES[index].second;
    }
  }
  return TypeId::INVALID;
}

Column Parser::TransformColumnDefinition(duckdb_libpgquery::PGColumnDef *cdef) const {
  string colname;
  if (cdef->colname != nullptr) {
    colname = cdef->colname;
  }
  TypeId type_id = TransformTypeId(cdef->typeName);
  return {colname, type_id};
}

Value Parser::TransformConstant(duckdb_libpgquery::PGAConst *c) const {
  duckdb_libpgquery::PGValue pg_val = c->val;

  switch (pg_val.type) {
    case duckdb_libpgquery::T_PGInteger:
      assert(pg_val.val.ival <= BUSTUB_INT32_MAX);
      return {TypeId::INTEGER, static_cast<int32_t>(pg_val.val.ival)};
    default:
      throw NotImplementedException(
          StringUtil::Format("Constant type %s is not implemented!", NodetypeToString(pg_val.type).c_str()));
  }
}

vector<Value> Parser::TransformExpressionList(duckdb_libpgquery::PGList *list) const {
  vector<Value> result;
  for (auto node = list->head; node != nullptr; node = node->next) {
    auto target = reinterpret_cast<duckdb_libpgquery::PGNode *>(node->data.ptr_value);
    assert(target);

    switch (target->type) {
      case duckdb_libpgquery::T_PGAConst: {
        auto val = TransformConstant(reinterpret_cast<duckdb_libpgquery::PGAConst *>(target));
        result.push_back(val);
        break;
      }
      case duckdb_libpgquery::T_PGRowExpr:
      default:
        throw NotImplementedException(
            StringUtil::Format("Expression type %s is not implemented!", NodetypeToString(target->type).c_str()));
        break;
    }
  }
  return result;
}

}  // namespace bustub
