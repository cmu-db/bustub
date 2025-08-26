//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager.cpp
//
// Identification: src/storage/disk/disk_manager.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
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
 * Creates a new disk manager that writes to the specified database file.
 * @param db_file the file name of the database file to write to
 */
DiskManager::DiskManager(const std::filesystem::path &db_file) : db_file_name_(db_file) {
  log_file_name_ = db_file_name_.filename().stem().string() + ".log";

  log_io_.open(log_file_name_, std::ios::binary | std::ios::in | std::ios::app | std::ios::out);
  // directory or file does not exist
  if (!log_io_.is_open()) {
    log_io_.clear();
    // create a new file
    log_io_.open(log_file_name_, std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
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
  assert(static_cast<size_t>(GetFileSize(db_file_name_)) >= page_capacity_ * BUSTUB_PAGE_SIZE);

  buffer_used = nullptr;
}

/**
 * Shut down the disk manager and close all the file resources.
 */
void DiskManager::ShutDown() {
  {
    std::scoped_lock scoped_db_io_latch(db_io_latch_);
    db_io_.close();
  }
  log_io_.close();
}

/**
 * Write the contents of the specified page into disk file
 */
void DiskManager::WritePage(page_id_t page_id, const char *page_data) {
  std::scoped_lock scoped_db_io_latch(db_io_latch_);
  size_t offset;
  if (pages_.find(page_id) != pages_.end()) {
    // Page already exists, overwrite it.
    offset = pages_[page_id];
  } else {
    // Page does not exist, allocate a new page. We use a free slot if available.
    offset = AllocatePage();
  }

  // Set the write cursor to the page offset.
  db_io_.seekp(offset);
  db_io_.write(page_data, BUSTUB_PAGE_SIZE);
  if (db_io_.bad()) {
    LOG_DEBUG("I/O error while writing page %d", page_id);
    return;
  }

  num_writes_ += 1;
  pages_[page_id] = offset;

  // Flush the write to disk.
  db_io_.flush();
}

/**
 * Read the contents of the specified page into the given memory area
 */
void DiskManager::ReadPage(page_id_t page_id, char *page_data) {
  std::scoped_lock scoped_db_io_latch(db_io_latch_);
  size_t offset;
  if (pages_.find(page_id) != pages_.end()) {
    offset = pages_[page_id];
  } else {
    // Page does not exist, allocate a new page. We use a free slot if available.
    offset = AllocatePage();
  }

  // Check if we have read beyond the file length.
  int file_size = GetFileSize(db_file_name_);
  if (file_size < 0) {
    LOG_DEBUG("I/O error: Fail to get db file size");
    return;
  }
  if (offset > static_cast<size_t>(file_size)) {
    LOG_DEBUG("I/O error: Read page %d past the end of file at offset %lu", page_id, offset);
    return;
  }

  pages_[page_id] = offset;

  // Set the read cursor to the page offset.
  db_io_.seekg(offset);
  db_io_.read(page_data, BUSTUB_PAGE_SIZE);

  if (db_io_.bad()) {
    LOG_DEBUG("I/O error while reading page %d", page_id);
    return;
  }

  // Check if the file ended before we could read a full page.
  int read_count = db_io_.gcount();
  if (read_count < BUSTUB_PAGE_SIZE) {
    LOG_DEBUG("I/O error: Read page %d hit the end of file at offset %lu, missing %d bytes", page_id, offset,
              BUSTUB_PAGE_SIZE - read_count);
    db_io_.clear();
    memset(page_data + read_count, 0, BUSTUB_PAGE_SIZE - read_count);
  }
}

/**
 * Note: This is a no-op for now without a more complex data structure to
 * track deallocated pages.
 */
void DiskManager::DeletePage(page_id_t page_id) {
  std::scoped_lock scoped_db_io_latch(db_io_latch_);
  if (pages_.find(page_id) == pages_.end()) {
    return;
  }

  size_t offset = pages_[page_id];
  free_slots_.push_back(offset);
  pages_.erase(page_id);
  num_deletes_ += 1;
}

/**
 * Write the contents of the log into disk file
 * Only return when sync is done, and only perform sequence write
 * @param log_data raw log data
 * @param size size of log entry
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
 * @param[out] log_data output buffer
 * @param size size of the log entry
 * @param offset offset of the log entry in the file
 * @return true if the read was successful, false otherwise
 */
auto DiskManager::ReadLog(char *log_data, int size, int offset) -> bool {
  if (offset >= GetFileSize(log_file_name_)) {
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

/** @return the number of disk flushes */
auto DiskManager::GetNumFlushes() const -> int { return num_flushes_; }

/** @return true iff the in-memory content has not been flushed yet */
auto DiskManager::GetFlushState() const -> bool { return flush_log_; }

/** @return the number of disk writes */
auto DiskManager::GetNumWrites() const -> int { return num_writes_; }

/** @return the number of deletions */
auto DiskManager::GetNumDeletes() const -> int { return num_deletes_; }

/**
 * Private helper function to get disk file size
 */
auto DiskManager::GetFileSize(const std::string &file_name) -> int {
  struct stat stat_buf;
  int rc = stat(file_name.c_str(), &stat_buf);
  return rc == 0 ? static_cast<int>(stat_buf.st_size) : -1;
}

/**
 * Allocate a page in a free slot. If no free slot is available, append to the end of the file.
 * @return the offset of the allocated page
 */
auto DiskManager::AllocatePage() -> size_t {
  // Find if there is a free slot in the file.
  if (!free_slots_.empty()) {
    auto offset = free_slots_.back();
    free_slots_.pop_back();
    return offset;
  }

  // Increase the file size if necessary.
  if (pages_.size() + 1 >= page_capacity_) {
    page_capacity_ *= 2;
    std::filesystem::resize_file(db_file_name_, (page_capacity_ + 1) * BUSTUB_PAGE_SIZE);
  }
  return pages_.size() * BUSTUB_PAGE_SIZE;
}

}  // namespace bustub
