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

  /** Grading function. Do not modify! */
  auto FetchPage(page_id_t page_id, bufferpool_callback_fn callback = nullptr) -> Page * {
    GradingCallback(callback, CallbackType::BEFORE, page_id);
    auto *result = FetchPgImp(page_id);
    GradingCallback(callback, CallbackType::AFTER, page_id);
    return result;
  }

  /** Grading function. Do not modify! */
  auto UnpinPage(page_id_t page_id, bool is_dirty, bufferpool_callback_fn callback = nullptr) -> bool {
    GradingCallback(callback, CallbackType::BEFORE, page_id);
    auto result = UnpinPgImp(page_id, is_dirty);
    GradingCallback(callback, CallbackType::AFTER, page_id);
    return result;
  }

  /** Grading function. Do not modify! */
  auto FlushPage(page_id_t page_id, bufferpool_callback_fn callback = nullptr) -> bool {
    GradingCallback(callback, CallbackType::BEFORE, page_id);
    auto result = FlushPgImp(page_id);
    GradingCallback(callback, CallbackType::AFTER, page_id);
    return result;
  }

  /** Grading function. Do not modify! */
  auto NewPage(page_id_t *page_id, bufferpool_callback_fn callback = nullptr) -> Page * {
    GradingCallback(callback, CallbackType::BEFORE, INVALID_PAGE_ID);
    auto *result = NewPgImp(page_id);
    GradingCallback(callback, CallbackType::AFTER, *page_id);
    return result;
  }

  /** Grading function. Do not modify! */
  auto DeletePage(page_id_t page_id, bufferpool_callback_fn callback = nullptr) -> bool {
    GradingCallback(callback, CallbackType::BEFORE, page_id);
    auto result = DeletePgImp(page_id);
    GradingCallback(callback, CallbackType::AFTER, page_id);
    return result;
  }

  /** Grading function. Do not modify! */
  void FlushAllPages(bufferpool_callback_fn callback = nullptr) {
    GradingCallback(callback, CallbackType::BEFORE, INVALID_PAGE_ID);
    FlushAllPgsImp();
    GradingCallback(callback, CallbackType::AFTER, INVALID_PAGE_ID);
  }

  /** @return size of the buffer pool */
  virtual auto GetPoolSize() -> size_t = 0;

 protected:
  /**
   * Grading function. Do not modify!
   * Invokes the callback function if it is not null.
   * @param callback callback function to be invoked
   * @param callback_type BEFORE or AFTER
   * @param page_id the page id to invoke the callback with
   */
  void GradingCallback(bufferpool_callback_fn callback, CallbackType callback_type, page_id_t page_id) {
    if (callback != nullptr) {
      callback(callback_type, page_id);
    }
  }

  /**
   * Fetch the requested page from the buffer pool.
   * @param page_id id of page to be fetched
   * @return the requested page
   */
  virtual auto FetchPgImp(page_id_t page_id) -> Page * = 0;

  /**
   * Unpin the target page from the buffer pool.
   * @param page_id id of page to be unpinned
   * @param is_dirty true if the page should be marked as dirty, false otherwise
   * @return false if the page pin count is <= 0 before this call, true otherwise
   */
  virtual auto UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool = 0;

  /**
   * Flushes the target page to disk.
   * @param page_id id of page to be flushed, cannot be INVALID_PAGE_ID
   * @return false if the page could not be found in the page table, true otherwise
   */
  virtual auto FlushPgImp(page_id_t page_id) -> bool = 0;

  /**
   * Creates a new page in the buffer pool.
   * @param[out] page_id id of created page
   * @return nullptr if no new pages could be created, otherwise pointer to new page
   */
  virtual auto NewPgImp(page_id_t *page_id) -> Page * = 0;

  /**
   * Deletes a page from the buffer pool.
   * @param page_id id of page to be deleted
   * @return false if the page exists but could not be deleted, true if the page didn't exist or deletion succeeded
   */
  virtual auto DeletePgImp(page_id_t page_id) -> bool = 0;

  /**
   * Flushes all the pages in the buffer pool to disk.
   */
  virtual void FlushAllPgsImp() = 0;
};
}  // namespace bustub
