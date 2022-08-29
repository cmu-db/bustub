//===----------------------------------------------------------------------===//
//                         BusTub
//
// bustub/binder/parser.h
//
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

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <string>

#include "binder/simplified_token.h"
#include "binder/sql_statement.h"
#include "binder/tokens.h"
#include "catalog/column.h"
#include "nodes/parsenodes.hpp"
#include "pg_definitions.hpp"
#include "postgres_parser.hpp"
#include "type/type_id.h"
#include "type/value.h"

namespace duckdb_libpgquery {
struct PGNode;
struct PGList;
}  // namespace duckdb_libpgquery

namespace bustub {
using std::vector;

//! The parser is responsible for parsing the query and converting it into a set
//! of parsed statements. The parsed statements can then be converted into a
//! plan and executed.
class Parser {
 public:
  Parser() = default;

  //! The parsed SQL statements from an invocation to ParseQuery.
  vector<unique_ptr<SQLStatement>> statements_;

  //! Attempts to parse a query into a series of SQL statements. Returns
  //! whether or not the parsing was successful. If the parsing was
  //! successful, the parsed statements will be stored in the statements
  //! variable.
  void ParseQuery(const string &query);

  //! Returns true if the given text matches a keyword of the parser
  static auto IsKeyword(const string &text) -> bool;

  //! Returns a list of all keywords in the parser
  static auto KeywordList() -> vector<ParserKeyword>;

  //! Tokenize a query, returning the raw tokens together with their locations
  static auto Tokenize(const string &query) -> vector<SimplifiedToken>;

  //! Transforms a Postgres parse tree into a set of SQL Statements
  auto TransformParseTree(duckdb_libpgquery::PGList *tree) const -> std::vector<std::unique_ptr<SQLStatement>>;

  auto NodetypeToString(duckdb_libpgquery::PGNodeTag type) const -> string;

  auto TransformColumnDefinition(duckdb_libpgquery::PGColumnDef *cdef) const -> Column;

  //! Transforms a Postgres statement into a single SQL statement
  auto TransformStatement(duckdb_libpgquery::PGNode *stmt) const -> unique_ptr<SQLStatement>;

  //! Transforms a Postgres statement into a single SQL statement
  auto TransformStatementInternal(duckdb_libpgquery::PGNode *stmt) const -> unique_ptr<SQLStatement>;

  //! Transform a Postgres TypeName string into a TypeId
  auto TransformTypeId(duckdb_libpgquery::PGTypeName *type_name) const -> TypeId;

  auto PGNodetypeToString(duckdb_libpgquery::PGNodeTag type) const -> std::string;

  auto TransformConstant(duckdb_libpgquery::PGAConst *c) const -> Value;

  auto TransformExpressionList(duckdb_libpgquery::PGList *list) const -> vector<Value>;
};

}  // namespace bustub
