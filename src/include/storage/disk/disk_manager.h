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

  ~DiskManager() = default;

  /**
   * Shut down the disk manager and close all the file resources.
   */
  void ShutDown();

  /**
   * Write a page to the database file.
   * @param page_id id of the page
   * @param page_data raw page data
   */
  void WritePage(page_id_t page_id, const char *page_data);

  /**
   * Read a page from the database file.
   * @param page_id id of the page
   * @param[out] page_data output buffer
   */
  void ReadPage(page_id_t page_id, char *page_data);

  /**
   * Append a log entry to the log file.
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
  bool ReadLog(char *log_data, int size, int offset);

  /**
   * Allocate a page on disk.
   * @return the id of the allocated page
   */
  page_id_t AllocatePage();

  /**
   * Deallocate a page on disk.
   * @param page_id id of the page to deallocate
   */
  void DeallocatePage(page_id_t page_id);

  /** @return the number of disk flushes */
  int GetNumFlushes() const;

  /** @return true iff the in-memory content has not been flushed yet */
  bool GetFlushState() const;

  /** @return the number of disk writes */
  int GetNumWrites() const;

  /**
   * Sets the future which is used to check for non-blocking flushes.
   * @param f the non-blocking flush check
   */
  inline void SetFlushLogFuture(std::future<void> *f) { flush_log_f_ = f; }

  /** Checks if the non-blocking flush future was set. */
  inline bool HasFlushLogFuture() { return flush_log_f_ != nullptr; }

 private:
  int GetFileSize(const std::string &file_name);
  // stream to write log file
  std::fstream log_io_;
  std::string log_name_;
  // stream to write db file
  std::fstream db_io_;
  std::string file_name_;
  std::atomic<page_id_t> next_page_id_;
  int num_flushes_;
  int num_writes_;
  bool flush_log_;
  std::future<void> *flush_log_f_;
};

}  // namespace bustub
