//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager.h
//
// Identification: src/include/storage/disk/disk_manager.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <fstream>
#include <future>  // NOLINT
#include <mutex>   // NOLINT
#include <string>

#include "common/config.h"

namespace bustub {

/**
 * DiskManager takes care of the allocation and deallocation of pages within a database. It performs the reading and
 * writing of pages to and from disk, providing a logical file layer within the context of a database management system.
 */
class DiskManager {
 public:
  /**
   * Creates a new disk manager that writes to the specified database file.
   * @param db_file the file name of the database file to write to
   */
  explicit DiskManager(const std::string &db_file);

  /** FOR TEST / LEADERBOARD ONLY, used by DiskManagerMemory */
  DiskManager() = default;

  virtual ~DiskManager() = default;

  /**
   * Shut down the disk manager and close all the file resources.
   */
  void ShutDown();

  /**
   * Write a page to the database file.
   * @param page_id id of the page
   * @param page_data raw page data
   */
  virtual void WritePage(page_id_t page_id, const char *page_data);

  /**
   * Read a page from the database file.
   * @param page_id id of the page
   * @param[out] page_data output buffer
   */
  virtual void ReadPage(page_id_t page_id, char *page_data);

  /**
   * Flush the entire log buffer into disk.
   * @param log_data raw log data
   * @param size size of log entry
   */
  void WriteLog(char *log_data, int size);

  /**
   * Read a log entry from the log file.
   * @param[out] log_data output buffer
   * @param size size of the log entry
   * @param offset offset of the log entry in the file
   * @return true if the read was successful, false otherwise
   */
  auto ReadLog(char *log_data, int size, int offset) -> bool;

  /** @return the number of disk flushes */
  auto GetNumFlushes() const -> int;

  /** @return true iff the in-memory content has not been flushed yet */
  auto GetFlushState() const -> bool;

  /** @return the number of disk writes */
  auto GetNumWrites() const -> int;

  /**
   * Sets the future which is used to check for non-blocking flushes.
   * @param f the non-blocking flush check
   */
  inline void SetFlushLogFuture(std::future<void> *f) { flush_log_f_ = f; }

  /** Checks if the non-blocking flush future was set. */
  inline auto HasFlushLogFuture() -> bool { return flush_log_f_ != nullptr; }

 protected:
  auto GetFileSize(const std::string &file_name) -> int;
  // stream to write log file
  std::fstream log_io_;
  std::string log_name_;
  // stream to write db file
  std::fstream db_io_;
  std::string file_name_;
  int num_flushes_{0};
  int num_writes_{0};
  bool flush_log_{false};
  std::future<void> *flush_log_f_{nullptr};
  // With multiple buffer pool instances, need to protect file access
  std::mutex db_io_latch_;
};

}  // namespace bustub
