//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager.h
//
// Identification: src/include/storage/disk/disk_manager_memory.h
//
// Copyright (c) 2015-2020, Carnegie Mellon University Database Group
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
class DiskManagerMemory {
 public:
  /**
   * Creates a new disk manager that writes to the specified database file.
   * @param db_file the file name of the database file to write to
   */
  DiskManagerMemory();

  ~DiskManagerMemory() = default;

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

 private:
  char *memory_;
  // stream to write db file
  std::atomic<page_id_t> next_page_id_;
  int num_flushes_;
  int num_writes_;
};

}  // namespace bustub
