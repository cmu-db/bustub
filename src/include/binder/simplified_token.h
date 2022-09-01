//===----------------------------------------------------------------------===//
//                         BusTub
//
// bustub/binder/simplified_token.h
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

#include <iostream>
#include <string>

namespace bustub {

/** Simplified tokens are a simplified (dense) representation of the lexer
 * used for simple syntax highlighting in the tests. */
enum class SimplifiedTokenType : uint8_t {
  SIMPLIFIED_TOKEN_IDENTIFIER,
  SIMPLIFIED_TOKEN_NUMERIC_CONSTANT,
  SIMPLIFIED_TOKEN_STRING_CONSTANT,
  SIMPLIFIED_TOKEN_OPERATOR,
  SIMPLIFIED_TOKEN_KEYWORD,
  SIMPLIFIED_TOKEN_COMMENT
};

struct SimplifiedToken {
  SimplifiedTokenType type_;
  int32_t start_;
};

enum class KeywordCategory : uint8_t { KEYWORD_RESERVED, KEYWORD_UNRESERVED, KEYWORD_TYPE_FUNC, KEYWORD_COL_NAME };

struct ParserKeyword {
  std::string name_;
  KeywordCategory category_;
};

}  // namespace bustub
