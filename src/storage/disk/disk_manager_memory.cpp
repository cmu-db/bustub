//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager_memory.cpp
//
// Identification: src/storage/disk/disk_manager_memory.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/disk/disk_manager_memory.h"

#include <cassert>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>  // NOLINT

#include "common/exception.h"
#include "common/logger.h"
#include "common/macros.h"

namespace bustub {

/**
 * Constructor: used for memory based manager
 */
DiskManagerMemory::DiskManagerMemory(size_t capacity) {
  page_capacity_ = capacity;
  memory_ = new char[capacity * BUSTUB_PAGE_SIZE];
}

/**
 * Write a page to the database file.
 * @param page_id id of the page
 * @param page_data raw page data
 */
void DiskManagerMemory::WritePage(page_id_t page_id, const char *page_data) {
  BUSTUB_ASSERT(static_cast<size_t>(page_id) < page_capacity_,
                "Ran out of disk space for limited memory disk manager implementation");
  size_t offset = static_cast<size_t>(page_id) * BUSTUB_PAGE_SIZE;
  // set write cursor to offset
  num_writes_ += 1;
  memcpy(memory_ + offset, page_data, BUSTUB_PAGE_SIZE);
}

/**
 * Read a page from the database file.
 * @param page_id id of the page
 * @param[out] page_data output buffer
 */
void DiskManagerMemory::ReadPage(page_id_t page_id, char *page_data) {
  int64_t offset = static_cast<int64_t>(page_id) * BUSTUB_PAGE_SIZE;
  memcpy(page_data, memory_ + offset, BUSTUB_PAGE_SIZE);
}

DiskManagerUnlimitedMemory::DiskManagerUnlimitedMemory() {
  std::scoped_lock l(mutex_);
  while (data_.size() < page_capacity_ + 1) {
    data_.push_back(std::make_shared<ProtectedPage>());
  }
  std::fill(recent_access_.begin(), recent_access_.end(), -1);
}

/**
 * Write a page to the database file.
 * @param page_id id of the page
 * @param page_data raw page data
 */
void DiskManagerUnlimitedMemory::WritePage(page_id_t page_id, const char *page_data) {
  if (page_id < 0) {
    fmt::println(stderr, "read invalid page {}", page_id);
    std::terminate();
    return;
  }

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
  num_writes_ += 1;

  PostProcessLatency(page_id);
}

/**
 * Read a page from the database file.
 * @param page_id id of the page
 * @param[out] page_data output buffer
 */
void DiskManagerUnlimitedMemory::ReadPage(page_id_t page_id, char *page_data) {
  if (page_id < 0) {
    fmt::println(stderr, "read invalid page {}", page_id);
    std::terminate();
    return;
  }

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
  std::shared_lock<std::shared_mutex> l_page(ptr->second);
  l.unlock();

  memcpy(page_data, ptr->first.data(), BUSTUB_PAGE_SIZE);

  PostProcessLatency(page_id);
}

/**
 * Delete a page from the database file. Reclaim the disk space.
 * Since we are using memory, this is a no-op.
 * @param page_id id of the page
 */
void DiskManagerUnlimitedMemory::DeletePage(page_id_t page_id) {
  // no-op since we are using memory
}

void DiskManagerUnlimitedMemory::ProcessLatency(page_id_t page_id) {
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

void DiskManagerUnlimitedMemory::PostProcessLatency(page_id_t page_id) {
  if (latency_simulator_enabled_) {
    std::scoped_lock<std::mutex> lck(latency_processor_mutex_);
    recent_access_[access_ptr_] = page_id;
    access_ptr_ = (access_ptr_ + 1) % recent_access_.size();
  }
}

auto DiskManagerUnlimitedMemory::GetLastReadThreadAndClear() -> std::optional<std::thread::id> {
  std::unique_lock<std::mutex> lck(mutex_);
  auto t = thread_id_;
  thread_id_ = std::nullopt;
  return t;
}

}  // namespace bustub
