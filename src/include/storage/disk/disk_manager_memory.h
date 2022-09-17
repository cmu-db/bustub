//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager_memory.h
//
// Identification: src/include/storage/disk/disk_manager_memory.h
//
// Copyright (c) 2015-2020, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <fstream>
#include <future>  // NOLINT
#include <string>

#include "common/config.h"
#include "storage/disk/disk_manager.h"

namespace bustub {

/**
 * DiskManagerMemory replicates the utility of DiskManager on memory. It is primarily used for
 * data structure performance testing.
 */
class DiskManagerMemory : public DiskManager {
 public:
  explicit DiskManagerMemory(size_t pages);

  ~DiskManagerMemory() override { delete[] memory_; }

  /**
   * Write a page to the database file.
   * @param page_id id of the page
   * @param page_data raw page data
   */
  void WritePage(page_id_t page_id, const char *page_data) override;

  /**
   * Read a page from the database file.
   * @param page_id id of the page
   * @param[out] page_data output buffer
   */
  void ReadPage(page_id_t page_id, char *page_data) override;

 private:
  char *memory_;
};

}  // namespace bustub
