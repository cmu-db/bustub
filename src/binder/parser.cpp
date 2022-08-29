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

#include <fmt/core.h>
#include <iostream>
#include <unordered_set>
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
using duckdb::PostgresParser;
using duckdb_libpgquery::PGKeywordCategory;
using duckdb_libpgquery::PGSimplifiedTokenType;

void Parser::ParseQuery(const std::string &query) {
  PostgresParser parser;
  parser.Parse(query);
  if (!parser.success) {
    LOG_INFO("Query failed to parse!");
    throw Exception(fmt::format("Query failed to parse: {}", parser.error_message));
    return;
  }

  if (parser.parse_tree == nullptr) {
    LOG_INFO("parser received empty statement");
    return;
  }

  try {
    // if it succeeded, we transform the Postgres parse tree into a list of
    // SQLStatements
    statements_ = TransformParseTree(parser.parse_tree);
  } catch (Exception &ex) {
    LOG_ERROR("Experienced an error when transforming the Postgres parse tree into BusTub statements.");
  }

  if (!statements_.empty()) {
    auto &last_statement = statements_.back();
    last_statement->stmt_length_ = query.size() - last_statement->stmt_location_;
    for (auto &statement : statements_) {
      statement->query_ = query;
    }
  }
}

auto Parser::IsKeyword(const std::string &text) -> bool { return PostgresParser::IsKeyword(text); }

auto Parser::KeywordList() -> std::vector<ParserKeyword> {
  auto keywords = PostgresParser::KeywordList();
  std::vector<ParserKeyword> result;
  for (auto &kw : keywords) {
    ParserKeyword res;
    res.name_ = kw.text;
    switch (kw.category) {
      case duckdb_libpgquery::PGKeywordCategory::PG_KEYWORD_RESERVED:
        res.category_ = KeywordCategory::KEYWORD_RESERVED;
        break;
      case duckdb_libpgquery::PGKeywordCategory::PG_KEYWORD_UNRESERVED:
        res.category_ = KeywordCategory::KEYWORD_UNRESERVED;
        break;
      case duckdb_libpgquery::PGKeywordCategory::PG_KEYWORD_TYPE_FUNC:
        res.category_ = KeywordCategory::KEYWORD_TYPE_FUNC;
        break;
      case duckdb_libpgquery::PGKeywordCategory::PG_KEYWORD_COL_NAME:
        res.category_ = KeywordCategory::KEYWORD_COL_NAME;
        break;
      default:
        throw Exception("Unrecognized keyword category");
    }
    result.push_back(res);
  }
  return result;
}

auto Parser::Tokenize(const std::string &query) -> std::vector<SimplifiedToken> {
  auto pg_tokens = PostgresParser::Tokenize(query);
  std::vector<SimplifiedToken> result;
  result.reserve(pg_tokens.size());
  for (auto &pg_token : pg_tokens) {
    SimplifiedToken token;
    switch (pg_token.type) {
      case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_IDENTIFIER:
        token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_IDENTIFIER;
        break;
      case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_NUMERIC_CONSTANT:
        token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_NUMERIC_CONSTANT;
        break;
      case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_STRING_CONSTANT:
        token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_STRING_CONSTANT;
        break;
      case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_OPERATOR:
        token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_OPERATOR;
        break;
      case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_KEYWORD:
        token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_KEYWORD;
        break;
      // comments are not supported by our tokenizer right now
      case duckdb_libpgquery::PGSimplifiedTokenType::PG_SIMPLIFIED_TOKEN_COMMENT:
        token.type_ = SimplifiedTokenType::SIMPLIFIED_TOKEN_COMMENT;
        break;
      default:
        throw Exception("Unrecognized token category");
    }
    token.start_ = pg_token.start;
    result.push_back(token);
  }
  return result;
}

}  // namespace bustub
