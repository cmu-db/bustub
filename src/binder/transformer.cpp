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

#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/statement/create_statement.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "common/exception.h"
#include "common/logger.h"
#include "common/util/string_util.h"
#include "type/decimal_type.h"

namespace bustub {

auto Binder::TransformParseTree(const Catalog &catalog, duckdb_libpgquery::PGList *tree) const
    -> std::vector<std::unique_ptr<BoundStatement>> {
  std::vector<std::unique_ptr<BoundStatement>> statements;
  for (auto entry = tree->head; entry != nullptr; entry = entry->next) {
    auto stmt = TransformStatement(catalog, static_cast<duckdb_libpgquery::PGNode *>(entry->data.ptr_value));
    statements.push_back(move(stmt));
  }
  return statements;
}

auto Binder::TransformStatement(const Catalog &catalog, duckdb_libpgquery::PGNode *stmt) const
    -> std::unique_ptr<BoundStatement> {
  switch (stmt->type) {
    case duckdb_libpgquery::T_PGRawStmt: {
      auto raw_stmt = reinterpret_cast<duckdb_libpgquery::PGRawStmt *>(stmt);
      auto result = TransformStatement(catalog, raw_stmt->stmt);
      if (result) {
        result->stmt_location_ = raw_stmt->stmt_location;
        result->stmt_length_ = raw_stmt->stmt_len;
      }
      return result;
    }
    case duckdb_libpgquery::T_PGCreateStmt:
      return std::make_unique<CreateStatement>(reinterpret_cast<duckdb_libpgquery::PGCreateStmt *>(stmt));
    case duckdb_libpgquery::T_PGInsertStmt:
      return std::make_unique<InsertStatement>(reinterpret_cast<duckdb_libpgquery::PGInsertStmt *>(stmt));
    case duckdb_libpgquery::T_PGSelectStmt:
      return std::make_unique<SelectStatement>(catalog, reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(stmt));
    case duckdb_libpgquery::T_PGDeleteStmt:
      return std::make_unique<DeleteStatement>(reinterpret_cast<duckdb_libpgquery::PGDeleteStmt *>(stmt));
    case duckdb_libpgquery::T_PGIndexStmt:
    case duckdb_libpgquery::T_PGUpdateStmt:
    default:
      throw NotImplementedException(NodeTagToString(stmt->type));
  }
}

}  // namespace bustub
