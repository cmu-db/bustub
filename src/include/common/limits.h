//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// limits.h
//
// Identification: src/include/type/limits.h
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
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

#include <cfloat>
#include <climits>
#include <cstdint>
#include <limits>

namespace bustub {

static constexpr double DBL_LOWEST = std::numeric_limits<double>::lowest();
static constexpr double FLT_LOWEST = std::numeric_limits<float>::lowest();

static constexpr int8_t BUSTUB_INT8_MIN = (SCHAR_MIN + 1);
static constexpr int16_t BUSTUB_INT16_MIN = (SHRT_MIN + 1);
static constexpr int32_t BUSTUB_INT32_MIN = (INT_MIN + 1);
static constexpr int64_t BUSTUB_INT64_MIN = (LLONG_MIN + 1);
static constexpr double BUSTUB_DECIMAL_MIN = FLT_LOWEST;
static constexpr uint64_t BUSTUB_TIMESTAMP_MIN = 0;
static constexpr uint32_t BUSTUB_DATE_MIN = 0;
static constexpr int8_t BUSTUB_BOOLEAN_MIN = 0;

static constexpr int8_t BUSTUB_INT8_MAX = SCHAR_MAX;
static constexpr int16_t BUSTUB_INT16_MAX = SHRT_MAX;
static constexpr int32_t BUSTUB_INT32_MAX = INT_MAX;
static constexpr int64_t BUSTUB_INT64_MAX = LLONG_MAX;
static constexpr uint64_t BUSTUB_UINT64_MAX = ULLONG_MAX - 1;
static constexpr double BUSTUB_DECIMAL_MAX = DBL_MAX;
static constexpr uint64_t BUSTUB_TIMESTAMP_MAX = 11231999986399999999U;
static constexpr uint64_t BUSTUB_DATE_MAX = INT_MAX;
static constexpr int8_t BUSTUB_BOOLEAN_MAX = 1;

static constexpr uint32_t BUSTUB_VALUE_NULL = UINT_MAX;
static constexpr int8_t BUSTUB_INT8_NULL = SCHAR_MIN;
static constexpr int16_t BUSTUB_INT16_NULL = SHRT_MIN;
static constexpr int32_t BUSTUB_INT32_NULL = INT_MIN;
static constexpr int64_t BUSTUB_INT64_NULL = LLONG_MIN;
static constexpr uint64_t BUSTUB_DATE_NULL = 0;
static constexpr uint64_t BUSTUB_TIMESTAMP_NULL = ULLONG_MAX;
static constexpr double BUSTUB_DECIMAL_NULL = DBL_LOWEST;
static constexpr int8_t BUSTUB_BOOLEAN_NULL = SCHAR_MIN;

static constexpr uint32_t BUSTUB_VARCHAR_MAX_LEN = UINT_MAX;

// Use to make TEXT type as the alias of VARCHAR(TEXT_MAX_LENGTH)
static constexpr uint32_t BUSTUB_TEXT_MAX_LEN = 1000000000;

// Objects (i.e., VARCHAR) with length prefix of -1 are NULL
static constexpr int OBJECTLENGTH_NULL = -1;
}  // namespace bustub
