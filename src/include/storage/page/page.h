//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// page.h
//
// Identification: src/include/storage/page/page.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstring>
#include <iostream>

#include "common/config.h"
#include "common/rwlatch.h"

namespace bustub {

/**
 * Wrapper around actual data page in main memory and also contains bookkeeping
 * information used by buffer pool manager like pin_count/dirty_flag/page_id.
 * Use page as a basic unit within the database system
 */
class Page {
  friend class BufferPoolManager;

 public:
  /**
   * Constructor. Zeros out the page data.
   */
  Page() { ResetMemory(); }

  /**
   * Destructor.
   */
  ~Page() = default;

  /**
   * Get actual data page content
   */
  inline char *GetData() { return data_; }

  /**
   * Get page id
   */
  inline page_id_t GetPageId() { return page_id_; }

  /**
   * Get page pin count
   */
  inline int GetPinCount() { return pin_count_; }

  /**
   * Get is dirty flag
   */
  inline bool IsDirty() { return is_dirty_; }

  /**
   * Acquire a write latch
   */
  inline void WLatch() { rwlatch_.WLock(); }

  /**
   * Release a write latch
   */
  inline void WUnlatch() { rwlatch_.WUnlock(); }

  /**
   * Acquire a read latch
   */
  inline void RLatch() { rwlatch_.RLock(); }

  /**
   * Release a read latch
   */
  inline void RUnlatch() { rwlatch_.RUnlock(); }

  /**
   * Get the page lsn
   */
  inline lsn_t GetLSN() { return *reinterpret_cast<lsn_t *>(GetData() + 4); }

  /**
   * Set the page lsn
   */
  inline void SetLSN(lsn_t lsn) { memcpy(GetData() + 4, &lsn, 4); }

 private:
  // method used by buffer pool manager
  inline void ResetMemory() { memset(data_, 0, PAGE_SIZE); }
  // members
  char data_[PAGE_SIZE];  // actual data
  page_id_t page_id_ = INVALID_PAGE_ID;
  int pin_count_ = 0;
  bool is_dirty_ = false;
  ReaderWriterLatch rwlatch_;
};

}  // namespace bustub
