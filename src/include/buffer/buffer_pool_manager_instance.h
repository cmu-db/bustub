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
#include "buffer/lru_replacer.h"
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
   * Creates a new BufferPoolManagerInstance.
   * @param pool_size the size of the buffer pool
   * @param disk_manager the disk manager
   * @param log_manager the log manager (for testing only: nullptr = disable logging)
   */
  BufferPoolManagerInstance(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager = nullptr);

  /**
   * Destroys an existing BufferPoolManagerInstance.
   */
  ~BufferPoolManagerInstance() override;

  /** @return size of the buffer pool */
  auto GetPoolSize() -> size_t override { return pool_size_; }

  /** @return pointer to all the pages in the buffer pool */
  auto GetPages() -> Page * { return pages_; }

 protected:
  /**
   * Fetch the requested page from the buffer pool.
   * @param page_id id of page to be fetched
   * @return the requested page
   */
  auto FetchPgImp(page_id_t page_id) -> Page * override;

  /**
   * Unpin the target page from the buffer pool.
   * @param page_id id of page to be unpinned
   * @param is_dirty true if the page should be marked as dirty, false otherwise
   * @return false if the page pin count is <= 0 before this call, true otherwise
   */
  auto UnpinPgImp(page_id_t page_id, bool is_dirty) -> bool override;

  /**
   * Flushes the target page to disk.
   * @param page_id id of page to be flushed, cannot be INVALID_PAGE_ID
   * @return false if the page could not be found in the page table, true otherwise
   */
  auto FlushPgImp(page_id_t page_id) -> bool override;

  /**
   * Creates a new page in the buffer pool.
   * @param[out] page_id id of created page
   * @return nullptr if no new pages could be created, otherwise pointer to new page
   */
  auto NewPgImp(page_id_t *page_id) -> Page * override;

  /**
   * Deletes a page from the buffer pool.
   * @param page_id id of page to be deleted
   * @return false if the page exists but could not be deleted, true if the page didn't exist or deletion succeeded
   */
  auto DeletePgImp(page_id_t page_id) -> bool override;

  /**
   * Flushes all the pages in the buffer pool to disk.
   */
  void FlushAllPgsImp() override;

  /**
   * Allocate a page on disk. Caller should acquire the latch before calling this function.
   * @return the id of the allocated page
   */
  auto AllocatePage() -> page_id_t;

  /**
   * Deallocate a page on disk. Caller should acquire the latch before calling this function.
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
  Replacer *replacer_;
  /** List of free pages. */
  std::list<frame_id_t> free_list_;
  /** This latch protects shared data structures. We recommend updating this comment to describe what it protects. */
  std::mutex latch_;
};
}  // namespace bustub
