//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// keyword_helper.cpp
//
// Identification: src/binder/keyword_helper.cpp
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

#include "binder/keyword_helper.h"
#include "binder/binder.h"
#include "common/util/string_util.h"

namespace bustub {

/** Return true if the given text matches a keyword of the parser. */
auto KeywordHelper::IsKeyword(const std::string &text) -> bool { return Binder::IsKeyword(text); }

/** Return true if the given std::string needs to be quoted when written as an identifier. */
auto KeywordHelper::RequiresQuotes(const std::string &text) -> bool {
  for (size_t i = 0; i < text.size(); i++) {
    if (i > 0 && (text[i] >= '0' && text[i] <= '9')) {
      continue;
    }
    if (text[i] >= 'a' && text[i] <= 'z') {
      continue;
    }
    if (text[i] == '_') {
      continue;
    }
    return true;
  }
  return IsKeyword(text);
}

/** Writes a std::string that is optionally quoted + escaped so it can be used as an identifier */
auto KeywordHelper::WriteOptionallyQuoted(const std::string &text, char quote) -> std::string {
  if (!RequiresQuotes(text)) {
    return text;
  }
  return std::string(1, quote) + StringUtil::Replace(text, std::string(1, quote), std::string(2, quote)) +
         std::string(1, quote);
}

}  // namespace bustub
