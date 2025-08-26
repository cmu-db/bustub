//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager.h
//
// Identification: src/include/storage/disk/disk_manager.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <filesystem>
#include <fstream>
#include <future>  // NOLINT
#include <mutex>   // NOLINT
#include <string>
#include <unordered_map>
#include <vector>

#include "common/config.h"
#include "common/logger.h"

namespace bustub {

/**
 * DiskManager takes care of the allocation and deallocation of pages within a database. It performs the reading and
 * writing of pages to and from disk, providing a logical file layer within the context of a database management system.
 *
 * DiskManager uses lazy allocation, meaning that it only allocates space on disk when it is first accessed. It
 * maintains a mapping of page ids to their corresponding offsets in the database file. When a page is deleted, it is
 * marked as free and can be reused by future allocations.
 */
class DiskManager {
 public:
  explicit DiskManager(const std::filesystem::path &db_file);

  /** FOR TEST / LEADERBOARD ONLY, used by DiskManagerMemory */
  DiskManager() = default;

  virtual ~DiskManager() = default;

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
   * Delete a page from the database file. Reclaim the disk space.
   * @param page_id id of the page
   */
  virtual void DeletePage(page_id_t page_id);

  void WriteLog(char *log_data, int size);

  auto ReadLog(char *log_data, int size, int offset) -> bool;

  auto GetNumFlushes() const -> int;

  auto GetFlushState() const -> bool;

  auto GetNumWrites() const -> int;

  auto GetNumDeletes() const -> int;

  /**
   * Sets the future which is used to check for non-blocking flushes.
   * @param f the non-blocking flush check
   */
  inline void SetFlushLogFuture(std::future<void> *f) { flush_log_f_ = f; }

  /** Checks if the non-blocking flush future was set. */
  inline auto HasFlushLogFuture() -> bool { return flush_log_f_ != nullptr; }

  /** @brief returns the log file name */
  inline auto GetLogFileName() const -> std::filesystem::path { return log_file_name_; }

  /** @brief returns the size of disk space in use */
  auto GetDbFileSize() -> size_t {
    auto file_size = GetFileSize(db_file_name_);
    if (file_size < 0) {
      LOG_DEBUG("I/O error: Fail to get db file size");
      return -1;
    }
    return static_cast<size_t>(file_size);
  }

 protected:
  int num_flushes_{0};
  int num_writes_{0};
  int num_deletes_{0};

  /** @brief The capacity of the file used for storage on disk. */
  size_t page_capacity_{DEFAULT_DB_IO_SIZE};

 private:
  auto GetFileSize(const std::string &file_name) -> int;

  auto AllocatePage() -> size_t;

  // stream to write log file
  std::fstream log_io_;
  std::filesystem::path log_file_name_;
  // stream to write db file
  std::fstream db_io_;
  std::filesystem::path db_file_name_;

  // Records the offset of each page in the db file.
  std::unordered_map<page_id_t, size_t> pages_;
  // Records the free slots in the db file if pages are deleted, indicated by offset.
  std::vector<size_t> free_slots_;

  bool flush_log_{false};
  std::future<void> *flush_log_f_{nullptr};
  // With multiple buffer pool instances, need to protect file access
  std::mutex db_io_latch_;
};

}  // namespace bustub
