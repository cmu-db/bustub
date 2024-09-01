//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include "common/exception.h"
#include "common/macros.h"
#include "storage/page/page_guard.h"

namespace bustub {

/**
 * @brief Creates a new `BufferPoolManager` instance.
 *
 * TODO more docs.
 *
 * @param pool_size the size of the buffer pool
 * @param disk_manager the disk manager
 * @param replacer_k the LookBack constant k for the LRU-K replacer
 * @param log_manager the log manager (for testing only: nullptr = disable logging). Please ignore this for P1.
 */
BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, size_t replacer_k,
                                     LogManager *log_manager)
    : pool_size_(pool_size), disk_scheduler_(std::make_unique<DiskScheduler>(disk_manager)), log_manager_(log_manager) {
  // TODO(students): remove this line after you have implemented the buffer pool manager
  throw NotImplementedException(
      "BufferPoolManager is not implemented yet. If you have finished implementing BPM, please remove the throw "
      "exception line in `buffer_pool_manager.cpp`.");

  // TODO(cjtsui) why is this public??

  // we allocate a consecutive memory space for the buffer pool
  pages_ = new Page[pool_size_];
  replacer_ = std::make_unique<LRUKReplacer>(pool_size, replacer_k);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

/**
 * @brief Destroys the `BufferPoolManager`, freeing up all memory that the buffer pool was using.
 */
BufferPoolManager::~BufferPoolManager() { delete[] pages_; }

/**
 * @brief Returns the number of frames that this buffer pool manages.
 */
auto BufferPoolManager::Size() -> size_t { return pool_size_; }

/**
 * @brief Allocates a new page on disk.
 *
 * ### Implementation
 *
 * You will maintain a thread-safe, monotonically increasing counter in the form of a `std::atomic<page_id_t>`.
 * See the documentation on [atomics](https://en.cppreference.com/w/cpp/atomic/atomic) for more information.
 *
 * Also, make sure to read the documentation for `DeletePage`! You can assume that you will never run out of disk
 * space, so this function _cannot_ fail.
 *
 * TODO(P1): Add implementation.
 *
 * @return The page ID of the new page.
 */
auto BufferPoolManager::NewPage() -> page_id_t { return INVALID_PAGE_ID; }

/**
 * @brief Removes a page from the database, both on disk and in memory.
 *
 * If the page is pinned in the buffer pool, this function does nothing and returns `false`. Otherwise, this function
 * removes the page from memory (if possible) and disk, returning `true`.
 *
 * ### Implementation
 *
 * Ideally, we would want to ensure that all space on disk is used efficiently. That would mean the space that deleted
 * pages on disk used to occupy should somehow be made available to new pages allocated by `NewPage`.
 *
 * If you would like to attempt this, you are free to do so. However, for this implementation, you are allowed to
 * assume you will not run out of disk space and simply keep allocating disk space upwards in `NewPage`.
 *
 * For (nonexistent) style points, you can still call `DeallocatePage` in case you want to implement something slightly
 * more space-efficient in the future.
 *
 * Think about all of the places a page or a page's metadata could be, and use that to guide you on implementing this
 * function.
 *
 * TODO(P1): Add implementation.
 *
 * @param page_id The page ID of the page we want to delete.
 * @return `false` if the page exists but could not be deleted, `true` if the page didn't exist or deletion succeeded.
 */
auto BufferPoolManager::DeletePage(page_id_t page_id) -> bool { return false; }

/**
 * @brief Acquires a read-locked guard over a page of data. The user can specify an `AccessType` if needed.
 *
 * Page data can _only_ be accessed via page guards. Users of this `BufferPoolManager` are expected to acquire either a
 * `ReadPageGuard` or a `WritePageGuard` depending on the mode in which they would like to access the data, which
 * ensures that any access of data is thread-safe.
 *
 * There can be any number of `ReadPageGuard`s reading the same page of data at a time across different threads.
 * However, all data access must be immutable. If a user wants to mutate the page's data, they must acquire a
 * `WritePageGuard` with `FetchPageWrite` instead.
 *
 * # Implementation
 *
 * TODO(cjtsui)
 *
 * TODO(P1): Add implementation.
 *
 * @param page_id
 * @param access_type The type of page access.
 * @return ReadPageGuard A latch guard ensuring shared and read-only access to a page's data.
 */
auto BufferPoolManager::FetchPageRead(page_id_t page_id, AccessType access_type) -> ReadPageGuard {
  return {this, nullptr};
}

/**
 * @brief Acquires a write-locked guard over a page of data. The user can specify an `AccessType` if needed.
 *
 * Page data can _only_ be accessed via page guards. Users of this `BufferPoolManager` are expected to acquire either a
 * `ReadPageGuard` or a `WritePageGuard` depending on the mode in which they would like to access the data, which
 * ensures that any access of data is thread-safe.
 *
 * There can only be 1 `WritePageGuard` reading/writing a page at a time. This allows data access to be both immutable
 * and mutable, meaning the thread that owns the `WritePageGuard` is allowed to manipulate the page's data however they
 * want. If a user wants to have multiple threads reading the page at the same time, they must acquire a `ReadPageGuard`
 * with `FetchPageRead` instead.
 *
 * # Implementation
 *
 * TODO(cjtsui)
 *
 * TODO(P1): Add implementation.
 *
 * @param page_id
 * @param access_type The type of page access.
 * @return WritePageGuard A latch guard ensuring exclusive and mutable access to a page's data.
 */
auto BufferPoolManager::FetchPageWrite(page_id_t page_id, AccessType access_type) -> WritePageGuard {
  return {this, nullptr};
}

/**
 * @brief Flushes the input page's data to disk and returns `true`. If the page is not in memory, this function does
 * nothing and returns `false`.
 *
 * # Implementation
 *
 * You will want to use the `WritePage` method on `DiskManager` to flush a page's data to disk, **regardless** of if the
 * dirty flag for the frame is set or not. Make sure the dirty flag of the frame is correct after flushing.
 *
 * TODO(P1): Add implementation.
 *
 * @param page_id id of page to be flushed, cannot be INVALID_PAGE_ID
 * @return false if the page could not be found in the page table, true otherwise
 */
auto BufferPoolManager::FlushPage(page_id_t page_id) -> bool { return false; }

/**
 * @brief Flushes all of the pages in the buffer pool manager to disk.
 *
 * TODO(P1): Add implementation.
 */
void BufferPoolManager::FlushAllPages() {}

/**
 * TODO(cjtsui): This entire function should probably be removed.
 *
 * @brief Fetch the requested page from the buffer pool. Return nullptr if page_id needs to be fetched from the disk
 * but all frames are currently in use and not evictable (in another word, pinned).
 *
 * First search for page_id in the buffer pool. If not found, pick a replacement frame from either the free list or
 * the replacer (always find from the free list first), read the page from disk by scheduling a read DiskRequest with
 * disk_scheduler_->Schedule(), and replace the old page in the frame. Similar to NewPage(), if the old page is dirty,
 * you need to write it back to disk and update the metadata of the new page
 *
 * In addition, remember to disable eviction and record the access history of the frame like you did for NewPage().
 *
 * @param page_id id of page to be fetched
 * @param access_type type of access to the page, only needed for leaderboard tests.
 * @return nullptr if page_id cannot be fetched, otherwise pointer to the requested page
 */
auto BufferPoolManager::FetchPage(page_id_t page_id, [[maybe_unused]] AccessType access_type) -> Page * {
  return nullptr;
}

/**
 * TODO(cjtsui): This entire function should probably be removed.
 *
 * @brief Unpin the target page from the buffer pool. If page_id is not in the buffer pool or its pin count is already
 * 0, return false.
 *
 * Decrement the pin count of a page. If the pin count reaches 0, the frame should be evictable by the replacer.
 * Also, set the dirty flag on the page to indicate if the page was modified.
 *
 * @param page_id id of page to be unpinned
 * @param is_dirty true if the page should be marked as dirty, false otherwise
 * @param access_type type of access to the page, only needed for leaderboard tests.
 * @return false if the page is not in the page table or its pin count is <= 0 before this call, true otherwise
 */
auto BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty, [[maybe_unused]] AccessType access_type) -> bool {
  return false;
}

}  // namespace bustub
