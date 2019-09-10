//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// config.h
//
// Identification: src/include/common/config.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <chrono>  // NOLINT
#include <cstdint>

namespace bustub {

/** Cycle detection is performed every CYCLE_DETECTION_INTERVAL milliseconds. */
extern std::chrono::milliseconds CYCLE_DETECTION_INTERVAL;

/** True if logging should be enabled, false otherwise. */
extern std::atomic<bool> ENABLE_LOGGING;

/** If ENABLE_LOGGING is true, the log should be flushed to disk every LOG_TIMEOUT. */
extern std::chrono::duration<int64_t> LOG_TIMEOUT;

#define INVALID_PAGE_ID (-1)                                  // representing an invalid page id
#define INVALID_TXN_ID (-1)                                   // representing an invalid transaction id
#define INVALID_LSN (-1)                                      // representing an invalid log sequence number
#define HEADER_PAGE_ID 0                                      // the header page id
#define PAGE_SIZE 4096                                        // size of a data page in byte
#define LOG_BUFFER_SIZE ((BUFFER_POOL_SIZE + 1) * PAGE_SIZE)  // size of a log buffer in byte
#define BUCKET_SIZE 50                                        // size of extendible hash bucket
#define BUFFER_POOL_SIZE 10                                   // size of buffer pool

using frame_id_t = int32_t;  // frame id type
using page_id_t = int32_t;   // page id type
using txn_id_t = int32_t;    // transaction id type
using lsn_t = int32_t;       // log sequence number type
using oid_t = uint16_t;

}  // namespace bustub
