//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.h
//
// Identification: src/include/buffer/buffer_pool_manager.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <unordered_map>

#include "buffer/lru_replacer.h"
#include "recovery/log_manager.h"
#include "storage/disk/disk_manager.h"
#include "storage/page/page.h"

namespace bustub {

/**
 * BufferPoolManager reads disk pages to and from its internal buffer pool.
 */
class BufferPoolManager {
 public:
  enum class CallbackType { BEFORE, AFTER };
  using bufferpool_callback_fn = void (*)(enum CallbackType, const page_id_t page_id);

  BufferPoolManager() = default;
  /**
   * Destroys an existing BufferPoolManager.
   */
  virtual ~BufferPoolManager() = default;

  /**
   * Fetch the requested page from the buffer pool.
   * @param page_id id of page to be fetched
   * @return the requested page
   */
  virtual Page *FetchPage(page_id_t page_id, bufferpool_callback_fn callback = nullptr) = 0;

  /**
   * Unpin the target page from the buffer pool.
   * @param page_id id of page to be unpinned
   * @param is_dirty true if the page should be marked as dirty, false otherwise
   * @return false if the page pin count is <= 0 before this call, true otherwise
   */
  virtual bool UnpinPage(page_id_t page_id, bool is_dirty, bufferpool_callback_fn callback = nullptr) = 0;

  /**
   * Flushes the target page to disk.
   * @param page_id id of page to be flushed, cannot be INVALID_PAGE_ID
   * @return false if the page could not be found in the page table, true otherwise
   */
  virtual bool FlushPage(page_id_t page_id, bufferpool_callback_fn callback = nullptr) = 0;

  /**
   * Creates a new page in the buffer pool.
   * @param[out] page_id id of created page
   * @return nullptr if no new pages could be created, otherwise pointer to new page
   */
  virtual Page *NewPage(page_id_t *page_id, bufferpool_callback_fn callback = nullptr) = 0;

  /**
   * Deletes a page from the buffer pool.
   * @param page_id id of page to be deleted
   * @return false if the page exists but could not be deleted, true if the page didn't exist or deletion succeeded
   */
  virtual bool DeletePage(page_id_t page_id, bufferpool_callback_fn callback = nullptr) = 0;

  /**
   * Flushes all the pages in the buffer pool to disk.
   */
  virtual void FlushAllPages(bufferpool_callback_fn callback = nullptr) = 0;

  /** @return size of the buffer pool */
  virtual size_t GetPoolSize() = 0;
};
}  // namespace bustub
