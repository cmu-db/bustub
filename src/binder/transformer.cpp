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

#include <memory>
#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_order_by.h"
#include "binder/bound_statement.h"
#include "binder/expressions/bound_window.h"
#include "binder/statement/create_statement.h"
#include "binder/statement/delete_statement.h"
#include "binder/statement/explain_statement.h"
#include "binder/statement/index_statement.h"
#include "binder/statement/insert_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/statement/update_statement.h"
#include "binder/table_ref/bound_base_table_ref.h"
#include "common/exception.h"
#include "common/logger.h"
#include "common/util/string_util.h"
#include "nodes/nodes.hpp"
#include "nodes/parsenodes.hpp"
#include "type/decimal_type.h"

namespace bustub {

void Binder::SaveParseTree(duckdb_libpgquery::PGList *tree) {
  std::vector<std::unique_ptr<BoundStatement>> statements;
  for (auto entry = tree->head; entry != nullptr; entry = entry->next) {
    statement_nodes_.push_back(reinterpret_cast<duckdb_libpgquery::PGNode *>(entry->data.ptr_value));
  }
}

auto Binder::BindStatement(duckdb_libpgquery::PGNode *stmt) -> std::unique_ptr<BoundStatement> {
  switch (stmt->type) {
    case duckdb_libpgquery::T_PGRawStmt:
      return BindStatement(reinterpret_cast<duckdb_libpgquery::PGRawStmt *>(stmt)->stmt);
    case duckdb_libpgquery::T_PGCreateStmt:
      return BindCreate(reinterpret_cast<duckdb_libpgquery::PGCreateStmt *>(stmt));
    case duckdb_libpgquery::T_PGInsertStmt:
      return BindInsert(reinterpret_cast<duckdb_libpgquery::PGInsertStmt *>(stmt));
    case duckdb_libpgquery::T_PGSelectStmt:
      return BindSelect(reinterpret_cast<duckdb_libpgquery::PGSelectStmt *>(stmt));
    case duckdb_libpgquery::T_PGExplainStmt:
      return BindExplain(reinterpret_cast<duckdb_libpgquery::PGExplainStmt *>(stmt));
    case duckdb_libpgquery::T_PGDeleteStmt:
      return BindDelete(reinterpret_cast<duckdb_libpgquery::PGDeleteStmt *>(stmt));
    case duckdb_libpgquery::T_PGUpdateStmt:
      return BindUpdate(reinterpret_cast<duckdb_libpgquery::PGUpdateStmt *>(stmt));
    case duckdb_libpgquery::T_PGIndexStmt:
      return BindIndex(reinterpret_cast<duckdb_libpgquery::PGIndexStmt *>(stmt));
    case duckdb_libpgquery::T_PGVariableSetStmt:
      return BindVariableSet(reinterpret_cast<duckdb_libpgquery::PGVariableSetStmt *>(stmt));
    case duckdb_libpgquery::T_PGVariableShowStmt:
      return BindVariableShow(reinterpret_cast<duckdb_libpgquery::PGVariableShowStmt *>(stmt));
    case duckdb_libpgquery::T_PGTransactionStmt:
      return BindTransaction(reinterpret_cast<duckdb_libpgquery::PGTransactionStmt *>(stmt));
    default:
      throw NotImplementedException(NodeTagToString(stmt->type));
  }
}

auto Binder::BindWindowFrame(duckdb_libpgquery::PGWindowDef *window_spec, std::unique_ptr<BoundWindow> expr)
    -> std::unique_ptr<BoundWindow> {
  if ((window_spec->frameOptions & FRAMEOPTION_END_UNBOUNDED_PRECEDING) != 0 ||
      (window_spec->frameOptions & FRAMEOPTION_START_UNBOUNDED_FOLLOWING) != 0) {
    throw Exception("Window frames starting with unbounded following or ending in unbounded preceding make no sense");
  }

  const bool range_mode = (window_spec->frameOptions & FRAMEOPTION_RANGE) != 0;
  WindowBoundary start = WindowBoundary::INVALID;
  WindowBoundary end = WindowBoundary::INVALID;
  if ((window_spec->frameOptions & FRAMEOPTION_START_UNBOUNDED_PRECEDING) != 0) {
    start = WindowBoundary::UNBOUNDED_PRECEDING;
  } else if ((window_spec->frameOptions & FRAMEOPTION_START_VALUE_PRECEDING) != 0) {
    start = range_mode ? WindowBoundary::EXPR_PRECEDING_RANGE : WindowBoundary::EXPR_PRECEDING_ROWS;
  } else if ((window_spec->frameOptions & FRAMEOPTION_START_VALUE_FOLLOWING) != 0) {
    start = range_mode ? WindowBoundary::EXPR_FOLLOWING_RANGE : WindowBoundary::EXPR_FOLLOWING_ROWS;
  } else if ((window_spec->frameOptions & FRAMEOPTION_START_CURRENT_ROW) != 0) {
    start = range_mode ? WindowBoundary::CURRENT_ROW_RANGE : WindowBoundary::CURRENT_ROW_ROWS;
  }

  if ((window_spec->frameOptions & FRAMEOPTION_END_UNBOUNDED_FOLLOWING) != 0) {
    end = WindowBoundary::UNBOUNDED_FOLLOWING;
  } else if ((window_spec->frameOptions & FRAMEOPTION_END_VALUE_PRECEDING) != 0) {
    end = range_mode ? WindowBoundary::EXPR_PRECEDING_RANGE : WindowBoundary::EXPR_PRECEDING_ROWS;
  } else if ((window_spec->frameOptions & FRAMEOPTION_END_VALUE_FOLLOWING) != 0) {
    end = range_mode ? WindowBoundary::EXPR_FOLLOWING_RANGE : WindowBoundary::EXPR_FOLLOWING_ROWS;
  } else if ((window_spec->frameOptions & FRAMEOPTION_END_CURRENT_ROW) != 0) {
    end = range_mode ? WindowBoundary::CURRENT_ROW_RANGE : WindowBoundary::CURRENT_ROW_ROWS;
  }

  expr->SetStart(start);
  expr->SetEnd(end);
  return expr;
}

}  // namespace bustub
