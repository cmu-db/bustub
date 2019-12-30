//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// mock_buffer_pool_manager.h
//
// Identification: test/buffer/mock_buffer_pool_manager.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <unordered_map>

#include "../test/buffer/counter.h"
#include "buffer/buffer_pool_manager.h"

namespace bustub {

// Add callback functions on BufferPoolManager
class MockBufferPoolManager : public BufferPoolManager {
 public:
  enum class CallbackType { BEFORE, AFTER };
  using bufferpool_callback_fn = void (MockBufferPoolManager::*)(enum CallbackType type, FuncType func_type);

  MockBufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager = nullptr)
      : BufferPoolManager(pool_size, disk_manager, log_manager) {}

  void counter_callback(enum CallbackType type, FuncType func_type) {
    if (type == CallbackType::BEFORE) {
      counter.Reset();
    } else {
      switch (func_type) {
        case FuncType::FetchPage:
          counter.CheckFetchPage();
          break;
        case FuncType::UnpinPage:
          counter.CheckUnpinPage();
          break;
        case FuncType::FlushPage:
          counter.CheckFlushPage();
          break;
        case FuncType::NewPage:
          counter.CheckNewPage();
          break;
        case FuncType::DeletePage:
          counter.CheckDeletePage();
          break;
        case FuncType::FlushAllPages:
          counter.CheckFlushAllPages();
          break;
      }
    }
  }

  /** Grading function. Do not modify/call! */
  Page *FetchPage(page_id_t page_id, bufferpool_callback_fn callback = &MockBufferPoolManager::counter_callback) {
    GradingCallback(callback, CallbackType::BEFORE, FuncType::FetchPage, page_id);
    auto *result = FetchPageImpl(page_id);
    GradingCallback(callback, CallbackType::AFTER, FuncType::FetchPage, page_id);
    return result;
  }

  /** Grading function. Do not modify/call! */
  bool UnpinPage(page_id_t page_id, bool is_dirty,
                 bufferpool_callback_fn callback = &MockBufferPoolManager::counter_callback) {
    GradingCallback(callback, CallbackType::BEFORE, FuncType::UnpinPage, page_id);
    auto result = UnpinPageImpl(page_id, is_dirty);
    GradingCallback(callback, CallbackType::AFTER, FuncType::UnpinPage, page_id);
    return result;
  }

  /** Grading function. Do not modify/call! */
  bool FlushPage(page_id_t page_id, bufferpool_callback_fn callback = &MockBufferPoolManager::counter_callback) {
    GradingCallback(callback, CallbackType::BEFORE, FuncType::FlushPage, page_id);
    auto result = FlushPageImpl(page_id);
    GradingCallback(callback, CallbackType::AFTER, FuncType::FlushPage, page_id);
    return result;
  }

  /** Grading function. Do not modify/call! */
  Page *NewPage(page_id_t *page_id, bufferpool_callback_fn callback = &MockBufferPoolManager::counter_callback) {
    GradingCallback(callback, CallbackType::BEFORE, FuncType::NewPage, INVALID_PAGE_ID);
    auto *result = NewPageImpl(page_id);
    GradingCallback(callback, CallbackType::AFTER, FuncType::NewPage, *page_id);
    return result;
  }

  /** Grading function. Do not modify/call! */
  bool DeletePage(page_id_t page_id, bufferpool_callback_fn callback = &MockBufferPoolManager::counter_callback) {
    GradingCallback(callback, CallbackType::BEFORE, FuncType::DeletePage, page_id);
    auto result = DeletePageImpl(page_id);
    GradingCallback(callback, CallbackType::AFTER, FuncType::DeletePage, page_id);
    return result;
  }

  /** Grading function. Do not modify/call! */
  void FlushAllPages(bufferpool_callback_fn callback = &MockBufferPoolManager::counter_callback) {
    GradingCallback(callback, CallbackType::BEFORE, FuncType::FlushAllPages, INVALID_PAGE_ID);
    FlushAllPagesImpl();
    GradingCallback(callback, CallbackType::AFTER, FuncType::FlushAllPages, INVALID_PAGE_ID);
  }

 private:
  /**
   * Grading function. Do not modify!
   * Invokes the callback function if it is not null.
   * @param callback callback function to be invoked
   * @param callback_type BEFORE or AFTER
   * @param page_id the page id to invoke the callback with
   */
  void GradingCallback(bufferpool_callback_fn callback, CallbackType callback_type, FuncType func_type,
                       page_id_t page_id) {
    if (callback != nullptr) {
      (this->*callback)(callback_type, func_type);
    }
  }

  /**
   * Fetch the requested page from the buffer pool.
   * @param page_id id of page to be fetched
   * @return the requested page
   */
  Page *FetchPageImpl(page_id_t page_id) {
    counter.AddCount(FuncType::FetchPage);
    return BufferPoolManager::FetchPageImpl(page_id);
  }

  /**
   * Unpin the target page from the buffer pool.
   * @param page_id id of page to be unpinned
   * @param is_dirty true if the page should be marked as dirty, false otherwise
   * @return false if the page pin count is <= 0 before this call, true otherwise
   */
  bool UnpinPageImpl(page_id_t page_id, bool is_dirty) {
    counter.AddCount(FuncType::UnpinPage);
    return BufferPoolManager::UnpinPageImpl(page_id, is_dirty);
  }

  /**
   * Flushes the target page to disk.
   * @param page_id id of page to be flushed, cannot be INVALID_PAGE_ID
   * @return false if the page could not be found in the page table, true otherwise
   */
  bool FlushPageImpl(page_id_t page_id) {
    counter.AddCount(FuncType::FlushPage);
    return BufferPoolManager::FlushPageImpl(page_id);
  }

  /**
   * Creates a new page in the buffer pool.
   * @param[out] page_id id of created page
   * @return nullptr if no new pages could be created, otherwise pointer to new page
   */
  Page *NewPageImpl(page_id_t *page_id) {
    counter.AddCount(FuncType::NewPage);
    return BufferPoolManager::NewPageImpl(page_id);
  }

  /**
   * Deletes a page from the buffer pool.
   * @param page_id id of page to be deleted
   * @return false if the page exists but could not be deleted, true if the page didn't exist or deletion succeeded
   */
  bool DeletePageImpl(page_id_t page_id) {
    counter.AddCount(FuncType::DeletePage);
    return BufferPoolManager::DeletePageImpl(page_id);
  }

  /**
   * Flushes all the pages in the buffer pool to disk.
   */
  void FlushAllPagesImpl() {
    counter.AddCount(FuncType::FlushAllPages);
    BufferPoolManager::FlushAllPagesImpl();
  }

  // For grading. Do not modify!
  Counter counter;
  /** Number of pages in the buffer pool. */
  size_t pool_size_;
  /** Array of buffer pool pages. */
  Page *pages_;
  /** Pointer to the disk manager. */
  DiskManager *disk_manager_ __attribute__((__unused__));
  /** Pointer to the log manager. */
  LogManager *log_manager_ __attribute__((__unused__));
  /** Page table for keeping track of buffer pool pages. */
  std::unordered_map<page_id_t, frame_id_t> page_table_;
  /** Replacer to find unpinned pages for replacement. */
  Replacer *replacer_;
  /** List of free pages. */
  std::list<page_id_t> free_list_;
  /** This latch protects shared data structures. We recommend updating this comment to describe what it protects. */
  std::mutex latch_;
};

}  // namespace bustub
