//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// log_recovery.h
//
// Identification: src/include/recovery/log_recovery.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <algorithm>
#include <mutex>  // NOLINT
#include <unordered_map>

#include "buffer/buffer_pool_manager.h"
#include "concurrency/lock_manager.h"
#include "recovery/log_record.h"

namespace bustub {

/**
 * Read log file from disk, redo and undo.
 */
class LogRecovery {
 public:
  LogRecovery(DiskManager *disk_manager, BufferPoolManager *buffer_pool_manager)
      : disk_manager_(disk_manager), buffer_pool_manager_(buffer_pool_manager), offset_(0) {
    log_buffer_ = new char[LOG_BUFFER_SIZE];
  }

  ~LogRecovery() {
    delete[] log_buffer_;
    log_buffer_ = nullptr;
  }

  void Redo();
  void Undo();
  bool DeserializeLogRecord(const char *data, LogRecord *log_record);

 private:
  DiskManager *disk_manager_ __attribute__((__unused__));
  BufferPoolManager *buffer_pool_manager_ __attribute__((__unused__));

  /** Maintain active transactions and its corresponding latest lsn. */
  std::unordered_map<txn_id_t, lsn_t> active_txn_;
  /** Mapping the log sequence number to log file offset for undos. */
  std::unordered_map<lsn_t, int> lsn_mapping_;

  int offset_ __attribute__((__unused__));
  char *log_buffer_;
};

}  // namespace bustub
