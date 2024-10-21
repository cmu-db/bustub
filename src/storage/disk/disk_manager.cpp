//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager.cpp
//
// Identification: src/storage/disk/disk_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <sys/stat.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <mutex>  // NOLINT
#include <string>
#include <thread>  // NOLINT

#include "common/config.h"
#include "common/exception.h"
#include "common/logger.h"
#include "storage/disk/disk_manager.h"

namespace bustub {

static char *buffer_used;

/**
 * Constructor: open/create a single database file & log file
 * @input db_file: database file name
 */
DiskManager::DiskManager(const std::filesystem::path &db_file) : file_name_(db_file) {
  log_name_ = file_name_.filename().stem().string() + ".log";

  log_io_.open(log_name_, std::ios::binary | std::ios::in | std::ios::app | std::ios::out);
  // directory or file does not exist
  if (!log_io_.is_open()) {
    log_io_.clear();
    // create a new file
    log_io_.open(log_name_, std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
    if (!log_io_.is_open()) {
      throw Exception("can't open dblog file");
    }
  }

  std::scoped_lock scoped_db_io_latch(db_io_latch_);
  db_io_.open(db_file, std::ios::binary | std::ios::in | std::ios::out);
  // directory or file does not exist
  if (!db_io_.is_open()) {
    db_io_.clear();
    // create a new file
    db_io_.open(db_file, std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
    if (!db_io_.is_open()) {
      throw Exception("can't open db file");
    }
  }

  // Initialize the database file.
  std::filesystem::resize_file(db_file, (page_capacity_ + 1) * BUSTUB_PAGE_SIZE);
  assert(static_cast<size_t>(GetFileSize(file_name_)) >= page_capacity_ * BUSTUB_PAGE_SIZE);

  buffer_used = nullptr;
}

/**
 * Close all file streams
 */
void DiskManager::ShutDown() {
  {
    std::scoped_lock scoped_db_io_latch(db_io_latch_);
    db_io_.close();
  }
  log_io_.close();
}

/**
 * @brief Increases the size of the file to fit the specified number of pages.
 */
void DiskManager::IncreaseDiskSpace(size_t pages) {
  std::scoped_lock scoped_db_io_latch(db_io_latch_);

  if (pages < pages_) {
    return;
  }

  pages_ = pages;
  while (page_capacity_ < pages_) {
    page_capacity_ *= 2;
  }

  std::filesystem::resize_file(file_name_, (page_capacity_ + 1) * BUSTUB_PAGE_SIZE);

  assert(static_cast<size_t>(GetFileSize(file_name_)) >= page_capacity_ * BUSTUB_PAGE_SIZE);
}

/**
 * Write the contents of the specified page into disk file
 */
void DiskManager::WritePage(page_id_t page_id, const char *page_data) {
  std::scoped_lock scoped_db_io_latch(db_io_latch_);
  size_t offset = static_cast<size_t>(page_id) * BUSTUB_PAGE_SIZE;

  // Set the write cursor to the page offset.
  num_writes_ += 1;
  db_io_.seekp(offset);
  db_io_.write(page_data, BUSTUB_PAGE_SIZE);

  if (db_io_.bad()) {
    LOG_DEBUG("I/O error while writing");
    return;
  }

  // Flush the write to disk.
  db_io_.flush();
}

/**
 * Read the contents of the specified page into the given memory area
 */
void DiskManager::ReadPage(page_id_t page_id, char *page_data) {
  std::scoped_lock scoped_db_io_latch(db_io_latch_);
  int offset = page_id * BUSTUB_PAGE_SIZE;

  // Check if we have read beyond the file length.
  if (offset > GetFileSize(file_name_)) {
    LOG_DEBUG("I/O error: Read past the end of file at offset %d", offset);
    return;
  }

  // Set the read cursor to the page offset.
  db_io_.seekg(offset);
  db_io_.read(page_data, BUSTUB_PAGE_SIZE);

  if (db_io_.bad()) {
    LOG_DEBUG("I/O error while reading");
    return;
  }

  // Check if the file ended before we could read a full page.
  int read_count = db_io_.gcount();
  if (read_count < BUSTUB_PAGE_SIZE) {
    LOG_DEBUG("I/O error: Read hit the end of file at offset %d, missing %d bytes", offset,
              BUSTUB_PAGE_SIZE - read_count);
    db_io_.clear();
    memset(page_data + read_count, 0, BUSTUB_PAGE_SIZE - read_count);
  }
}

/**
 * Note: This is a no-op for now without a more complex data structure to
 * track deallocated pages.
 */
void DiskManager::DeletePage(page_id_t page_id) { num_deletes_ += 1; }

/**
 * Write the contents of the log into disk file
 * Only return when sync is done, and only perform sequence write
 */
void DiskManager::WriteLog(char *log_data, int size) {
  // enforce swap log buffer
  assert(log_data != buffer_used);
  buffer_used = log_data;

  if (size == 0) {  // no effect on num_flushes_ if log buffer is empty
    return;
  }

  flush_log_ = true;

  if (flush_log_f_ != nullptr) {
    // used for checking non-blocking flushing
    assert(flush_log_f_->wait_for(std::chrono::seconds(10)) == std::future_status::ready);
  }

  num_flushes_ += 1;
  // sequence write
  log_io_.write(log_data, size);

  // check for I/O error
  if (log_io_.bad()) {
    LOG_DEBUG("I/O error while writing log");
    return;
  }
  // needs to flush to keep disk file in sync
  log_io_.flush();
  flush_log_ = false;
}

/**
 * Read the contents of the log into the given memory area
 * Always read from the beginning and perform sequence read
 * @return: false means already reach the end
 */
auto DiskManager::ReadLog(char *log_data, int size, int offset) -> bool {
  if (offset >= GetFileSize(log_name_)) {
    // LOG_DEBUG("end of log file");
    // LOG_DEBUG("file size is %d", GetFileSize(log_name_));
    return false;
  }
  log_io_.seekp(offset);
  log_io_.read(log_data, size);

  if (log_io_.bad()) {
    LOG_DEBUG("I/O error while reading log");
    return false;
  }
  // if log file ends before reading "size"
  int read_count = log_io_.gcount();
  if (read_count < size) {
    log_io_.clear();
    memset(log_data + read_count, 0, size - read_count);
  }

  return true;
}

/**
 * Returns number of flushes made so far
 */
auto DiskManager::GetNumFlushes() const -> int { return num_flushes_; }

/**
 * Returns true if the log is currently being flushed
 */
auto DiskManager::GetFlushState() const -> bool { return flush_log_; }

/**
 * Returns number of Writes made so far
 */
auto DiskManager::GetNumWrites() const -> int { return num_writes_; }

/**
 * Returns number of deletions made so far
 */
auto DiskManager::GetNumDeletes() const -> int { return num_deletes_; }

/**
 * Private helper function to get disk file size
 */
auto DiskManager::GetFileSize(const std::string &file_name) -> int {
  struct stat stat_buf;
  int rc = stat(file_name.c_str(), &stat_buf);
  return rc == 0 ? static_cast<int>(stat_buf.st_size) : -1;
}

}  // namespace bustub
