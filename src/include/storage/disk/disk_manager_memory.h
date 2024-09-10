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

/** @brief The default size of the database file. */
static const size_t DEFAULT_DB_IO_SIZE = 16;

/**
 * DiskManagerMemory replicates the utility of DiskManager on memory. It is primarily used for
 * data structure performance testing.
 */
class DiskManagerMemory : public DiskManager {
 public:
  explicit DiskManagerMemory(size_t pages);

  ~DiskManagerMemory() override { delete[] memory_; }

  /**
   * This function should increase the disk space, but since we have a fixed amount of memory we just check that the
   * pages are in bounds.
   */
  void IncreaseDiskSpace(size_t pages) override;

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
  size_t pages_;
  char *memory_;
};

/**
 * DiskManagerMemory replicates the utility of DiskManager on memory. It is primarily used for
 * data structure performance testing.
 */
class DiskManagerUnlimitedMemory : public DiskManager {
 public:
  DiskManagerUnlimitedMemory() {
    std::scoped_lock l(mutex_);
    while (data_.size() < pages_ + 1) {
      data_.push_back(std::make_shared<ProtectedPage>());
    }
    std::fill(recent_access_.begin(), recent_access_.end(), -1);
  }

  /**
   * This function should increase the disk space, but since this is memory we just resize the vector.
   */
  void IncreaseDiskSpace(size_t pages) override {
    std::scoped_lock l(mutex_);

    if (pages < pages_) {
      return;
    }

    while (data_.size() < pages + 1) {
      data_.push_back(std::make_shared<ProtectedPage>());
    }
    assert(data_.size() == pages + 1);

    pages_ = pages;
  }

  /**
   * Write a page to the database file.
   * @param page_id id of the page
   * @param page_data raw page data
   */
  void WritePage(page_id_t page_id, const char *page_data) override {
    ProcessLatency(page_id);

    std::unique_lock<std::mutex> l(mutex_);
    if (!thread_id_.has_value()) {
      thread_id_ = std::this_thread::get_id();
    }
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

    PostProcessLatency(page_id);
  }

  /**
   * Read a page from the database file.
   * @param page_id id of the page
   * @param[out] page_data output buffer
   */
  void ReadPage(page_id_t page_id, char *page_data) override {
    ProcessLatency(page_id);

    std::unique_lock<std::mutex> l(mutex_);
    if (!thread_id_.has_value()) {
      thread_id_ = std::this_thread::get_id();
    }
    if (page_id >= static_cast<int>(data_.size()) || page_id < 0) {
      fmt::println(stderr, "page {} not in range", page_id);
      std::terminate();
      return;
    }
    if (data_[page_id] == nullptr) {
      fmt::println(stderr, "page {} not exist", page_id, pages_);
      std::terminate();
      return;
    }
    std::shared_ptr<ProtectedPage> ptr = data_[page_id];
    std::shared_lock<std::shared_mutex> l_page(ptr->second);
    l.unlock();

    memcpy(page_data, ptr->first.data(), BUSTUB_PAGE_SIZE);

    PostProcessLatency(page_id);
  }

  void ProcessLatency(page_id_t page_id) {
    uint64_t sleep_micro_sec = 1000;  // for random access, 1ms latency
    if (latency_simulator_enabled_) {
      std::unique_lock<std::mutex> lck(latency_processor_mutex_);
      for (auto &recent_page_id : recent_access_) {
        if ((recent_page_id & (~0x3)) == (page_id & (~0x3))) {
          sleep_micro_sec = 100;  // for access in the same "block", 0.1ms latency
          break;
        }
        if (page_id >= recent_page_id && page_id <= recent_page_id + 3) {
          sleep_micro_sec = 100;  // for sequential access, 0.1ms latency
          break;
        }
      }
      lck.unlock();
      std::this_thread::sleep_for(std::chrono::microseconds(sleep_micro_sec));
    }
  }

  void PostProcessLatency(page_id_t page_id) {
    if (latency_simulator_enabled_) {
      std::scoped_lock<std::mutex> lck(latency_processor_mutex_);
      recent_access_[access_ptr_] = page_id;
      access_ptr_ = (access_ptr_ + 1) % recent_access_.size();
    }
  }

  void EnableLatencySimulator(bool enabled) { latency_simulator_enabled_ = enabled; }

  auto GetLastReadThreadAndClear() -> std::optional<std::thread::id> {
    std::unique_lock<std::mutex> lck(mutex_);
    auto t = thread_id_;
    thread_id_ = std::nullopt;
    return t;
  }

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

  size_t pages_{DEFAULT_DB_IO_SIZE};
};

}  // namespace bustub
