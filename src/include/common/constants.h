//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// constants.h
//
// Identification: src/include/common/constants.h
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

#include <memory>
#include <string>
#include <vector>

namespace bustub {

// NOTE: there is a copy of this in the Postgres' parser grammar (gram.y)
#define DEFAULT_SCHEMA "main"
#define TEMP_SCHEMA "temp"
#define INVALID_SCHEMA ""

static constexpr const uint64_t INVALID_INDEX = -1;
static constexpr int INVALID_PAGE_ID = -1;                                    // invalid page id
static constexpr int INVALID_TXN_ID = -1;                                     // invalid transaction id
static constexpr int INVALID_LSN = -1;                                        // invalid log sequence number
static constexpr int HEADER_PAGE_ID = 0;                                      // the header page id
static constexpr int PAGE_SIZE = 4096;                                        // size of a data page in byte
static constexpr int BUFFER_POOL_SIZE = 10;                                   // size of buffer pool
static constexpr int LOG_BUFFER_SIZE = ((BUFFER_POOL_SIZE + 1) * PAGE_SIZE);  // size of a log buffer in byte
static constexpr int BUCKET_SIZE = 50;                                        // size of extendible hash bucket
static constexpr uint64_t COLUMN_IDENTIFIER_ROW_ID = static_cast<uint64_t>(-1);

// size of extendible hash bucket
//! inline std directives that we use frequently
using std::make_shared;
using std::make_unique;
using std::move;
using std::shared_ptr;
using std::string;
using std::unique_ptr;
using std::vector;

using frame_id_t = int32_t;    // frame id type
using page_id_t = int32_t;     // page id type
using txn_id_t = int32_t;      // transaction id type
using lsn_t = int32_t;         // log sequence number type
using slot_offset_t = size_t;  // slot offset type
using oid_t = uint16_t;

}  // namespace bustub
