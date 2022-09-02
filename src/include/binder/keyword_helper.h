//===----------------------------------------------------------------------===//
//                         BusTub
//
// binder/keyword_helper.h
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

#include <string>

#include "common/config.h"

namespace bustub {

class KeywordHelper {
 public:
  /** Return true if the given text matches a keyword of the parser. */
  static auto IsKeyword(const std::string &text) -> bool;

  /** Return true if the given std::string needs to be quoted when written as an identifier. */
  static auto RequiresQuotes(const std::string &text) -> bool;

  // Writes a std::string that is optionally quoted + escaped so it can be used as an identifier
  static auto WriteOptionallyQuoted(const std::string &text, char quote = '"') -> std::string;
};

}  // namespace bustub
