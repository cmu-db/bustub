//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager_memory.h
//
// Identification: src/include/storage/disk/disk_manager_memory.h
//
// Copyright (c) 2015-2024, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/disk/disk_manager.h"

#include <array>
#include <cassert>
#include <chrono>  // NOLINT
#include <cstring>
#include <fstream>
#include <future>  // NOLINT
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <shared_mutex>
#include <string>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "common/config.h"
#include "common/exception.h"
#include "common/logger.h"
#include "fmt/core.h"

namespace bustub {

/**
 * DiskManagerMemory replicates the utility of DiskManager on memory. It is primarily used for
 * data structure performance testing.
 */
class DiskManagerMemory : public DiskManager {
 public:
  explicit DiskManagerMemory(size_t capacity);

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

/**
 * DiskManagerMemory replicates the utility of DiskManager on memory. It is primarily used for
 * data structure performance testing.
 */
class DiskManagerUnlimitedMemory : public DiskManager {
 public:
  DiskManagerUnlimitedMemory();

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

  /**
   * Delete a page from the database file. Reclaim the disk space.
   * Since we are using memory, this is a no-op.
   * @param page_id id of the page
   */
  void DeletePage(page_id_t page_id) override;

  void ProcessLatency(page_id_t page_id);

  void PostProcessLatency(page_id_t page_id);

  void EnableLatencySimulator(bool enabled) { latency_simulator_enabled_ = enabled; }

  auto GetLastReadThreadAndClear() -> std::optional<std::thread::id>;

 private:
  bool latency_simulator_enabled_{false};

  std::mutex latency_processor_mutex_;
  std::array<page_id_t, 4> recent_access_;
  uint64_t access_ptr_{0};

  using Page = std::array<char, BUSTUB_PAGE_SIZE>;
  using ProtectedPage = std::pair<Page, std::shared_mutex>;

  std::mutex mutex_;
  std::optional<std::thread::id> thread_id_;
  std::vector<std::shared_ptr<ProtectedPage>> data_;
};

}  // namespace bustub
