//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager_memory.cpp
//
// Identification: src/storage/disk/disk_manager_memory.cpp
//
// Copyright (c) 2015-2020, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <sys/stat.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>  // NOLINT

#include "common/exception.h"
#include "common/logger.h"
#include "storage/disk/disk_manager_memory.h"

namespace bustub {

DiskManagerMemory::DiskManagerMemory() : next_page_id_(0) {
  num_flushes_ = 0;
  num_writes_ = 0;
  memory_ = static_cast<char *>(malloc(1 << 30));
}

void DiskManagerMemory::ShutDown() { free(memory_); }

void DiskManagerMemory::WritePage(page_id_t page_id, const char *page_data) {
  size_t offset = static_cast<size_t>(page_id) * PAGE_SIZE;
  // set write cursor to offset
  num_writes_ += 1;
  memcpy(memory_ + offset, page_data, PAGE_SIZE);
}

void DiskManagerMemory::ReadPage(page_id_t page_id, char *page_data) {
  int offset = page_id * PAGE_SIZE;
  // check if read beyond file length
  memcpy(page_data, memory_ + offset, PAGE_SIZE);
}
/**
 * Allocate new page (operations like create index/table)
 * For now just keep an increasing counter
 */
page_id_t DiskManagerMemory::AllocatePage() { return next_page_id_++; }

/**
 * Deallocate page (operations like drop index/table)
 * Need bitmap in header page for tracking pages
 * This does not actually need to do anything for now.
 */
void DiskManagerMemory::DeallocatePage(__attribute__((unused)) page_id_t page_id) {}

/**
 * Returns number of flushes made so far
 */
int DiskManagerMemory::GetNumFlushes() const { return num_flushes_; }

/**
 * Returns number of Writes made so far
 */
int DiskManagerMemory::GetNumWrites() const { return num_writes_; }

}  // namespace bustub
