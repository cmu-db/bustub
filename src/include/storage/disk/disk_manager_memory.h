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
#include <array>
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

/**
 * DiskManagerMemory replicates the utility of DiskManager on memory. It is primarily used for
 * data structure performance testing.
 */
class DiskManagerUnlimitedMemory : public DiskManager {
 public:
  DiskManagerUnlimitedMemory() = default;

  /**
   * Write a page to the database file.
   * @param page_id id of the page
   * @param page_data raw page data
   */
  void WritePage(page_id_t page_id, const char *page_data) override {
    if (latency_ > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(latency_));
    }

    std::unique_lock<std::mutex> l(mutex_);
    if (page_id >= static_cast<int>(data_.size())) {
      data_.resize(page_id + 1);
    }
    if (data_[page_id] == nullptr) {
      data_[page_id] = std::make_shared<ProtectedPage>();
    }
    std::shared_ptr<ProtectedPage> ptr = data_[page_id];
    std::unique_lock<std::shared_mutex> l_page(ptr->second);
    l.unlock();

    memcpy(ptr->first.data(), page_data, BUSTUB_PAGE_SIZE);
  }

  /**
   * Read a page from the database file.
   * @param page_id id of the page
   * @param[out] page_data output buffer
   */
  void ReadPage(page_id_t page_id, char *page_data) override {
    if (latency_ > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(latency_));
    }

    std::unique_lock<std::mutex> l(mutex_);
    if (page_id >= static_cast<int>(data_.size()) || page_id < 0) {
      LOG_WARN("page not exist");
      return;
    }
    if (data_[page_id] == nullptr) {
      LOG_WARN("page not exist");
      return;
    }
    std::shared_ptr<ProtectedPage> ptr = data_[page_id];
    std::shared_lock<std::shared_mutex> l_page(ptr->second);
    l.unlock();

    memcpy(page_data, ptr->first.data(), BUSTUB_PAGE_SIZE);
  }

  void SetLatency(size_t latency_ms) { latency_ = latency_ms; }

 private:
  std::mutex mutex_;
  using Page = std::array<char, BUSTUB_PAGE_SIZE>;
  using ProtectedPage = std::pair<Page, std::shared_mutex>;
  std::vector<std::shared_ptr<ProtectedPage>> data_;
  size_t latency_{0};
};

}  // namespace bustub
