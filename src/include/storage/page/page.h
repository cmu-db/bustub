//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// page.h
//
// Identification: src/include/storage/page/page.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstring>
#include <iostream>
#include <vector>

#include "common/config.h"
#include "common/rwlatch.h"

namespace bustub {

/**
 * @brief
 *
 * TODO(2024 tas) This entire class needs to be updated.
 *
 * Page is the basic unit of storage within the database system. Page provides a wrapper for actual data pages being
 * held in main memory. Page also contains book-keeping information that is used by the buffer pool manager, e.g.
 * pin count, dirty flag, page id, etc.
 */
class Page {
  // There is book-keeping information inside the page that should only be relevant to the buffer pool manager.
  friend class BufferPoolManager;
  friend class ReadPageGuard;
  friend class WritePageGuard;

 public:
  /** Constructor. Zeros out the page data. */
  Page() : data_(static_cast<size_t>(BUSTUB_PAGE_SIZE), 0), page_id_(INVALID_PAGE_ID) {
    pin_count_.store(0, std::memory_order_relaxed);
    is_dirty_.store(false, std::memory_order_relaxed);
    ResetMemory();
  }

  /** Default destructor. */
  ~Page() = default;

  /** @return the actual data contained within this page */
  inline auto GetData() -> char * { return data_.data(); }

  /** @return the page id of this page */
  inline auto GetPageId() const -> page_id_t { return page_id_; }

  /** @return the pin count of this page. */
  inline auto GetPinCount() const -> int { return pin_count_.load(std::memory_order_acquire); }

  /** @return true if the page in memory has been modified from the page on disk, false otherwise */
  inline auto IsDirty() const -> bool { return is_dirty_.load(std::memory_order_acquire); }

  /** Acquire the page write latch. */
  inline void WLatch() { rwlatch_.lock(); }

  /** Release the page write latch. */
  inline void WUnlatch() { rwlatch_.unlock(); }

  /** Acquire the page read latch. */
  inline void RLatch() { rwlatch_.lock_shared(); }

  /** Release the page read latch. */
  inline void RUnlatch() { rwlatch_.unlock_shared(); }

  /** @return the page LSN. */
  inline auto GetLSN() -> lsn_t { return *reinterpret_cast<lsn_t *>(GetData() + OFFSET_LSN); }

  /** Sets the page LSN. */
  inline void SetLSN(lsn_t lsn) { memcpy(GetData() + OFFSET_LSN, &lsn, sizeof(lsn_t)); }

 protected:
  static_assert(sizeof(page_id_t) == 4);
  static_assert(sizeof(lsn_t) == 4);

  static constexpr size_t SIZE_PAGE_HEADER = 8;
  static constexpr size_t OFFSET_PAGE_START = 0;
  static constexpr size_t OFFSET_LSN = 4;

 private:
  /** @brief Resets the frame.
   *
   * Zeroes out the data that is held within the frame and sets all fields to default values.
   */
  inline void ResetMemory() {
    std::fill(data_.begin(), data_.end(), 0);
    page_id_ = INVALID_PAGE_ID;
    pin_count_.store(0, std::memory_order_release);
    is_dirty_.store(false, std::memory_order_release);
  }

  /** @brief The actual data that is stored within a page.
   *
   * In practice, this should be stored as a `char data_[BUSTUB_PAGE_SIZE]`.
   * However, in order to allow address sanitizer to detect buffer overflow, we store it as a vector.
   *
   * Note that friend classes should make sure not increase the size of this data field.
   */
  std::vector<char> data_;

  /** @brief The ID of this page. */
  page_id_t page_id_;

  /** @brief The pin count of this page. */
  std::atomic<size_t> pin_count_;

  /** @brief True if the page is dirty, i.e. it is different from its corresponding page on disk. */
  std::atomic<bool> is_dirty_;

  /** @brief The page latch protecting data access. */
  std::shared_mutex rwlatch_;
};

}  // namespace bustub
