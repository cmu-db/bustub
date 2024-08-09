//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// write_back_cache.h
//
// Identification: src/include/storage/disk/write_back_cache.h
//
// Copyright (c) 2015-2024, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <cstring>
#include "common/config.h"
#include "common/macros.h"
#include "storage/page/page.h"

namespace bustub {

/**
 * WriteBackCache provides extra memory space other than the buffer pool to store the pages. It's purpose
 * is to gather the copy of pages that are about to be written back to disk, so that the bpm doesn't have
 * to incur IO penality and wait for the write to be completed when evicting.
 * Spring 24: The cache is limited to store a constant number of pages in total (8).
 * !! ANY ATTEMPTS TO ADD ANOTHER IN-MEMORY CACHE WILL BE REVIEWED MANUALLY AS PER LEADERBOARD POLICY !!
 */
class WriteBackCache {
 public:
  WriteBackCache() : write_back_pages_{new Page[8]} {}
  ~WriteBackCache() { delete[] write_back_pages_; }
  DISALLOW_COPY_AND_MOVE(WriteBackCache);

  /**
   * @brief Adds a new page to the write back cache.
   * @param page the page pointer from the bpm that is about to be evicted.
   * @return pointer to the copied page in the cache, or nullptr if the cache is full.
   */
  auto Add(Page *page) -> Page * {
    if ((page == nullptr) || IsFull()) {
      return nullptr;
    }

    uint32_t slot = FindFreeSlot();
    memcpy(write_back_pages_[slot].GetData(), page->GetData(), BUSTUB_PAGE_SIZE);
    MarkSlotUsed(slot);

    return write_back_pages_ + slot;
  }

  /**
   * @brief Removes a page from the write back cache.
   * @param page the pointer previously returned by Add.
   */
  auto Remove(Page *page) -> void {
    if (page != nullptr) {
      MarkSlotFree(page - write_back_pages_);
    }
  }

 private:
  /** @brief Whether the cache is full. */
  auto IsFull() -> bool { return free_slot_bitmap_ == 0xFFU; }

  /** @brief Finds a free slot in the cache, if not full. */
  auto FindFreeSlot() -> uint32_t {
    BUSTUB_ASSERT(!IsFull(), "no free slot in write back cache");
    uint32_t i = 0;
    uint8_t bitmap = free_slot_bitmap_;
    while ((bitmap & 1U) != 0) {
      bitmap >>= 1;
      i++;
    }
    return i;
  }

  /** @brief Marks a free slot as used. */
  void MarkSlotUsed(uint32_t slot) {
    BUSTUB_ASSERT(((free_slot_bitmap_ >> slot) & 1U) == 0, "slot has already been used");
    free_slot_bitmap_ |= (1U << slot);
  }

  /** @brief Marks a used slot as free. */
  void MarkSlotFree(uint32_t slot) {
    BUSTUB_ASSERT(((free_slot_bitmap_ >> slot) & 1U) == 1, "slot is already free");
    free_slot_bitmap_ &= ~(1U << slot);
  }

  /** The array of write back cache pages. */
  Page *write_back_pages_;
  /** The bitmap that records which slots are free. */
  uint8_t free_slot_bitmap_{0};
};

}  // namespace bustub
