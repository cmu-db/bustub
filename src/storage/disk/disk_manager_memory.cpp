//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager.cpp
//
// Identification: src/storage/disk/disk_manager.cpp
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

/**
 * Constructor: used for memory based manager
 */
DiskManagerMemory::DiskManagerMemory() { memory_ = static_cast<char *>(malloc(1 << 30)); }

/**
 * Write the contents of the specified page into disk file
 */
void DiskManagerMemory::WritePage(page_id_t page_id, const char *page_data) {
  size_t offset = static_cast<size_t>(page_id) * PAGE_SIZE;
  // set write cursor to offset
  num_writes_ += 1;
  memcpy(memory_ + offset, page_data, PAGE_SIZE);
}

/**
 * Read the contents of the specified page into the given memory area
 */
void DiskManagerMemory::ReadPage(page_id_t page_id, char *page_data) {
  int offset = page_id * PAGE_SIZE;
  memcpy(page_data, memory_ + offset, PAGE_SIZE);
}

}  // namespace bustub
