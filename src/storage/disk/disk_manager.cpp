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
#include <cstring>
#include <iostream>
#include <thread>  // NOLINT

#include "common/exception.h"
#include "common/logger.h"
#include "storage/disk/disk_manager.h"

namespace bustub {

static char *buffer_used;

/**
 * Constructor: open/create a single database file & log file
 * @input db_file: database file name
 */
DiskManager::DiskManager(const std::string &db_file) : file_name_(db_file) {
  std::string::size_type n = file_name_.rfind('.');
  if (n == std::string::npos) {
    LOG_DEBUG("wrong file format");
    return;
  }
  log_name_ = file_name_.substr(0, n) + ".log";

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
  buffer_used = nullptr;
}

/**
 * Close all file streams
 * Note: ShutDown should now be aware of none-terminated future flush
 */
void DiskManager::ShutDown() {
  has_shut_down_ = true;
  // Close the fstream, to prevent memory leak
  {
    std::scoped_lock scoped_db_io_latch(db_io_latch_);
    std::scoped_lock scoped_log_io_latch(log_io_latch_);
    db_io_.close();
    log_io_.close();
  }
  // Not for production code, only to check the state under DEBUG mode
  assert(!db_io_.is_open() && !log_io_.is_open());

  // Wait for the ever existing flushing operation to finish
  if (HasWriteLogFuture()) {
    // Same problem as in DiskManager::WriteLog(), may crash the program
    WaitForAsyncToComplete();
  }
  // Sanity Check
  assert(!HasWriteLogFuture());
}

/**
 * Write the contents of the specified page into disk file
 */
void DiskManager::WritePage(page_id_t page_id, const char *page_data) {
  std::scoped_lock scoped_db_io_latch(db_io_latch_);
  size_t offset = static_cast<size_t>(page_id) * BUSTUB_PAGE_SIZE;
  // set write cursor to offset
  num_writes_ += 1;
  db_io_.seekp(offset);
  db_io_.write(page_data, BUSTUB_PAGE_SIZE);
  // check for I/O error
  if (db_io_.bad()) {
    LOG_DEBUG("I/O error while writing");
    return;
  }
  // needs to flush to keep disk file in sync
  db_io_.flush();
}

/**
 * Read the contents of the specified page into the given memory area
 */
void DiskManager::ReadPage(page_id_t page_id, char *page_data) {
  std::scoped_lock scoped_db_io_latch(db_io_latch_);
  int offset = page_id * BUSTUB_PAGE_SIZE;
  // check if read beyond file length
  if (offset > GetFileSize(file_name_)) {
    LOG_DEBUG("I/O error reading past end of file");
    // std::cerr << "I/O error while reading" << std::endl;
  } else {
    // set read cursor to offset
    db_io_.seekp(offset);
    db_io_.read(page_data, BUSTUB_PAGE_SIZE);
    if (db_io_.bad()) {
      LOG_DEBUG("I/O error while reading");
      return;
    }
    // if file ends before reading BUSTUB_PAGE_SIZE
    int read_count = db_io_.gcount();
    if (read_count < BUSTUB_PAGE_SIZE) {
      LOG_DEBUG("Read less than a page");
      db_io_.clear();
      // std::cerr << "Read less than a page" << std::endl;
      memset(page_data + read_count, 0, BUSTUB_PAGE_SIZE - read_count);
    }
  }
}

/**
 * Write the contents of the log into disk file
 * Only return when sync is done, and only perform sequence write
 * Note: This version of WriteLog() will block until all asynchronous tasks are finished
 */
void DiskManager::WriteLog(char *log_data, int size) {
  std::scoped_lock scoped_log_io_latch(log_io_latch_);
  // Enforce swap log buffer
  assert(log_data != buffer_used);
  buffer_used = log_data;

  if (size == 0) {  // no effect on num_flushes_ if log buffer is empty
    return;
  }

  if (HasWriteLogFuture()) {
    // Used for checking non-blocking flushing, the default (and expected) time for a logging thread is 10s
    // TODO(Zihao): Now the program will crash if the flush is still persisting after 10s,
    // Should it be handled more softly (e.g, Print a warning and terminate just that thread by resetting the ptr?)
    WaitForAsyncToComplete();
  }

  // Write the log
  log_io_.write(log_data, size);

  // Check for I/O error
  if (log_io_.bad()) {
    LOG_DEBUG("I/O error while writing log");
    return;
  }

  // Flush the log
  log_io_.flush();

  // Update flush status
  flush_log_ = true;

  // Increase number of flushing by one
  num_flushes_ += 1;
}

/**
 * @brief A non-blocking, asynchronous version of WriteLog
 *
 * @param log_data
 * @param size
 */
void DiskManager::WriteLogAsync(char *log_data, int size) {
  auto curr_write_f = std::make_unique<std::future<void>>(std::async(std::launch::async, [&] {
    // First acquire the lock, the underlying threads will sequentially acquire the log_io_latch_
    std::scoped_lock scoped_log_io_latch(log_io_latch_);

    // Ensure swap log buffer
    if (log_data != buffer_used) {
      // Directly return
      // TODO(Zihao): Warning the user?
      return;
    }
    // Otherwise set the buffer_used to the incoming new log_data
    buffer_used = log_data;

    // No effect on num_flushes_ if log buffer is empty
    if (size == 0) {
      return;
    }

    // Write the log
    log_io_.write(log_data, size);

    // Check for I/O error
    if (log_io_.bad()) {
      // Directly return, may be changed in the future
      return;
    }

    // Flush the log to disk
    log_io_.flush();
    // The current flushing is done
    flush_log_ = true;
    // Increase the num of flush by one
    num_flushes_ += 1;
  }));

  // Set flush status to false, since the execution of this task is unknown
  flush_log_ = false;

  // Push the new future handle in to the deque
  PushWriteLogFuture(std::move(curr_write_f));
}

/**
 * Read the contents of the log into the given memory area
 * Always read from the beginning and perform sequence read
 * @return: false means already reach the end
 * Note: This function will read the log AFTER all write operations (including asynchronous) are finished
 */
auto DiskManager::ReadLog(char *log_data, int size, int offset) -> bool {
  std::scoped_lock scoped_log_io_latch(log_io_latch_);

  // Wait for potential async write operations
  if (HasWriteLogFuture()) {
    WaitForAsyncToComplete();
  }

  // Perform sanity check here
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
 * Private helper function to get disk file size
 */
auto DiskManager::GetFileSize(const std::string &file_name) -> int {
  struct stat stat_buf;
  int rc = stat(file_name.c_str(), &stat_buf);
  return rc == 0 ? static_cast<int>(stat_buf.st_size) : -1;
}

}  // namespace bustub
