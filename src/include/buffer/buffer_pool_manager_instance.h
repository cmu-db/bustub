//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_instance.h
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

#include "buffer/buffer_pool_manager.h"
#include "buffer/lru_k_replacer.h"
#include "common/config.h"
#include "container/hash/extendible_hash_table.h"
#include "recovery/log_manager.h"
#include "storage/disk/disk_manager.h"
#include "storage/page/page.h"

namespace bustub {

/**
 * BufferPoolManager reads disk pages to and from its internal buffer pool.
 */
class BufferPoolManagerInstance : public BufferPoolManager {
 public:
  /**
   * @brief Creates a new BufferPoolManagerInstance.
   * @param pool_size the size of the buffer pool
   * @param disk_manager the disk manager
   * @param replacer_k the lookback constant k for the LRU-K replacer
   * @param log_manager the log manager (for testing only: nullptr = disable logging)
   */
  BufferPoolManagerInstance(size_t pool_size, DiskManager *disk_manager, size_t replacer_k = LRUK_REPLACER_K,
                            LogManager *log_manager = nullptr);

  /**
   * @brief Destroy an existing BufferPoolManagerInstance.
   */
  ~BufferPoolManagerInstance() override;

  /** @brief Return the size (number of frames) of the buffer pool. */
  auto GetPoolSize() -> size_t override { return pool_size_; }

  /** @brief Return the pointer to all the pages in the buffer pool. */
  auto GetPages() -> Page * { return pages_; }

 protected:
  /**
   * TODO(P1): Add implementation
   *
   * @brief Fetch the requested page from the buffer pool.
   *
   * 1. Search for the page_id in the page table first.
   * 1.1 If found, return the page immediately.
   * 1.2 If not found, find a replacement page from either the free list
   * or the replacer. (NOTE: always find from the free list first)
   * 2. If the replacement page is dirty, write it back to disk by calling DiskManager::WritePage().
   * 3. Delete the replacement page from the page table and insert the new page.
   * 4. Update the new page metadata, read page content from disk and return the page pointer.
   *
   * Before returning, remember to "Pin" the frame by calling replacer.SetEvitable(frame_id, false)
   * so that the replacer wouldn't evict the frame before the bpm "Unpin"s it.
   * You also need to record the access history of the frame in the replacer for the lru-k algorithm to work.
   *
   * @param page_id id of page to be fetched
   * @return the requested page
   */
  auto FetchPgImp(page_id_t page_id) -> Page * override;

  /**
   * TODO(P1): Add implementation
   *
   * @brief Unpin the target page from the buffer pool.
   *
   * Decrement the pin count of a page. If the pin count reaches 0, the frame should be evictable by the replacer.
   * Also set the dirty flag on the page to indicate if the page was modified.
   *
   * @param page_id id of page to be unpinned
   * @param is_dirty true if the page should be marked as dirty, false otherwise
   * @return False if the page is not in the page table or its pin count is <= 0 before this call, true otherwise
   */
  auto UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool override;

  /**
   * TODO(P1): Add implementation
   *
   * @brief Flush the target page to disk.
   *
   * Use the DiskManager::WritePage() method to flush a page to disk.
   * Unset the dirty flag of the page after flushing.
   *
   * @param page_id id of page to be flushed, cannot be INVALID_PAGE_ID
   * @return False if the page could not be found in the page table, true otherwise
   */
  auto FlushPgImp(page_id_t page_id) -> bool override;

  /**
   * TODO(P1): Add implementation
   *
   * @brief Create a new page in the buffer pool.
   *
   * 1. Call AllocatePage() to get the page id for the new page
   * 2. Pick a replacement page from either the free list or the replacer. (NOTE: always find from the free list first)
   * 3. If the replacement page is dirty, write it back to disk by calling DiskManager::WritePage().
   * 4. Delete the replacement page from the page table and insert the new page.
   * 5. Update the new page metadata, zero out the memory and return the page pointer.
   *
   * Same as FetchPage(), remember to pin and record the access of the frame in the replacer.
   *
   * @param[out] page_id id of created page
   * @return nullptr if no new pages could be created, otherwise pointer to new page
   */
  auto NewPgImp(page_id_t *page_id) -> Page * override;

  /**
   * TODO(P1): Add implementation
   *
   * @brief Delete a page from the buffer pool.
   *
   * 1. Delete the page from the page table.
   * 2. Remove the frame from the replacer.
   * 3. Add the frame back to the free list.
   * 4. Reset the page's metadata (id, memory, dirty flag).
   * 4. Call DeallocatePage() to imitate freeing the page on disk.
   *
   * @param page_id id of page to be deleted
   * @return False if the page exists but could not be deleted, true if the page didn't exist or deletion succeeded
   */
  auto DeletePgImp(page_id_t page_id) -> bool override;

  /**
   * TODO(P1): Add implementation
   *
   * @brief Flush all the pages in the buffer pool to disk.
   */
  void FlushAllPgsImp() override;

  /**
   * @brief Allocate a page on disk. Caller should acquire the latch before calling this function.
   * @return the id of the allocated page
   */
  auto AllocatePage() -> page_id_t;

  /**
   * @brief Deallocate a page on disk. Caller should acquire the latch before calling this function.
   * @param page_id id of the page to deallocate
   */
  void DeallocatePage(__attribute__((unused)) page_id_t page_id) {
    // This is a no-nop right now without a more complex data structure to track deallocated pages
  }

  /** Number of pages in the buffer pool. */
  const size_t pool_size_;
  /** The next page id to be allocated  */
  std::atomic<page_id_t> next_page_id_ = 0;
  /** Bucket size for the extendible hash table */
  const size_t bucket_size_ = 4;

  /** Array of buffer pool pages. */
  Page *pages_;
  /** Pointer to the disk manager. */
  DiskManager *disk_manager_ __attribute__((__unused__));
  /** Pointer to the log manager. */
  LogManager *log_manager_ __attribute__((__unused__));
  /** Page table for keeping track of buffer pool pages. */
  ExtendibleHashTable<page_id_t, frame_id_t> *page_table_;
  /** Replacer to find unpinned pages for replacement. */
  LRUKReplacer *replacer_;
  /** List of free frames that don't have any pages on them. */
  std::list<frame_id_t> free_list_;
  /** This latch protects shared data structures. We recommend updating this comment to describe what it protects. */
  std::mutex latch_;

  // TODO(student): You may add additional private members and helper functions
};
}  // namespace bustub
