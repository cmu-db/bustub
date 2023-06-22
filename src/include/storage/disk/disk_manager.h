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
#include <deque>
#include <fstream>
#include <future>  // NOLINT
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <utility>

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

  virtual ~DiskManager() {
    // If ShutDown() is not manually called, shut it down when DiskManager goes out of scope
    if (!has_shut_down_.load()) {
      ShutDown();
    }
  }

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
   * The non-blocking, asynchronous version of WriteLog()
   * @param log_data
   * @param size
   */
  void WriteLogAsync(char *log_data, int size);

  /**
   * Read a log entry from the log file.
   * @param[out] log_data output buffer
   * @param size size of the log entry
   * @param offset offset of the log entry in the file
   * @return true if the read was successful, false otherwise
   */
  auto ReadLog(char *log_data, int size, int offset) -> bool;

  /**
   * @brief Returns number of flushes made so far
   * @return The number of disk flushes
   */
  auto GetNumFlushes() const -> int { return num_flushes_.load(); };

  /**
   * @brief Returns true if the log is currently being flushed
   * @return True iff the in-memory content has currently been flushed
   */
  auto GetFlushState() const -> bool { return flush_log_.load(); };

  /**
   * @brief Returns number of Writes made so far
   * @return The number of disk writes
   */
  auto GetNumWrites() const -> int { return num_writes_; };

  /**
   * Sets the future which is used to check for non-blocking writes.
   * @param f the non-blocking write check
   */
  inline void PushWriteLogFuture(std::unique_ptr<std::future<void>> f) {
    std::scoped_lock write_log_f_deque_latch(write_log_f_deque_latch_);
    write_log_f_deque_.push_back(std::move(f));
  }

  inline void PopWriteLogFuture() {
    std::scoped_lock write_log_f_deque_latch(write_log_f_deque_latch_);
    write_log_f_deque_.pop_front();
  }

  /** Checks if the non-blocking write future exists. */
  inline auto HasWriteLogFuture() -> bool {
    std::scoped_lock lock(write_log_f_deque_latch_);
    return !write_log_f_deque_.empty();
  }

  inline void WaitForAsyncToComplete() {
    std::scoped_lock lock(write_log_f_deque_latch_);
    while (!write_log_f_deque_.empty()) {
      if (write_log_f_deque_.front()->valid()) {
        // Be consistent with previous solution
        assert(write_log_f_deque_.front()->wait_for(std::chrono::seconds(10)) == std::future_status::ready);
      }
      write_log_f_deque_.pop_front();
    }
  }

 protected:
  auto GetFileSize(const std::string &file_name) -> int;

  /** Stream to write log file */
  std::fstream log_io_;
  std::string log_name_;

  /** Stream to write db file */
  std::fstream db_io_;
  std::string file_name_;

  std::atomic<int> num_flushes_{0};
  int num_writes_{0};
  std::atomic<bool> flush_log_{false};
  /** Used for Destructor */
  std::atomic<bool> has_shut_down_{false};

  /** For asynchronous functions */
  std::deque<std::unique_ptr<std::future<void>>> write_log_f_deque_;
  std::mutex write_log_f_deque_latch_;

  /** With multiple buffer pool instances, need to protect file access */
  std::mutex db_io_latch_;

  /** Same as above, the log access also needs to be protected, since std::fstream is NOT thread-safe */
  std::mutex log_io_latch_;
};

}  // namespace bustub
