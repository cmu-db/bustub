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
#include <memory>
#include <mutex>  // NOLINT
#include <unordered_map>

#include "buffer/lru_k_replacer.h"
#include "common/config.h"
#include "recovery/log_manager.h"
#include "storage/disk/disk_scheduler.h"
#include "storage/disk/write_back_cache.h"
#include "storage/page/page.h"
#include "storage/page/page_guard.h"

namespace bustub {

/**
 * BufferPoolManager reads disk pages to and from its internal buffer pool.
 */
class BufferPoolManager {
 public:
  BufferPoolManager(size_t pool_size, DiskManager *disk_manager, size_t replacer_k = LRUK_REPLACER_K,
                    LogManager *log_manager = nullptr);
  ~BufferPoolManager();
  auto Size() -> size_t;
  auto NewPage() -> page_id_t;
  auto DeletePage(page_id_t page_id) -> bool;
  auto ReadPage(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> std::optional<ReadPageGuard>;
  auto WritePage(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> std::optional<WritePageGuard>;
  auto FlushPage(page_id_t page_id) -> bool;
  void FlushAllPages();

  // TODO(cjtsui) deprecate everything under this comment. Leaving it in for now so it doesn't break everything.
  auto FetchPageRead(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> ReadPageGuard;
  auto FetchPageWrite(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> WritePageGuard;
  auto FetchPage(page_id_t page_id, AccessType access_type = AccessType::Unknown) -> Page *;
  auto UnpinPage(page_id_t page_id, bool is_dirty, AccessType access_type = AccessType::Unknown) -> bool;

 private:
  /** @brief The number of frames in the buffer pool. */
  const size_t pool_size_;

  /** @brief The next page ID to be allocated.  */
  std::atomic<page_id_t> next_page_id_ = 0;

  /**
   * @brief The latch protecting most of the `BufferPoolManager`'s shared data structures.
   *
   * TODO(p1): We would recommend updating this comment to describe exactly what this latch protects.
   */
  std::mutex latch_;

  /** @brief A large array of buffer pool frames.
   *
   * TODO(cjtsui) We should probably rename this. These are technically not pages but frames.
   * We should probably also get rename the `Page` class to `FrameHeader` as well and makes changes there too.
   */
  Page *pages_;

  /** @brief A pointer to the disk scheduler. */
  std::unique_ptr<DiskScheduler> disk_scheduler_ __attribute__((__unused__));

  /** @brief The page table that keeps track of the mapping between pages and buffer pool frames. */
  std::unordered_map<page_id_t, frame_id_t> page_table_;

  /** @brief The replacer to find unpinned/candidate pages for eviction. */
  std::unique_ptr<LRUKReplacer> replacer_;

  /** @brief A list of free frames that do not hold a page's data. */
  std::list<frame_id_t> free_list_;

  /**
   * @brief A write-back cache.
   *
   * Note: You may want to use this cache to optimize the write requests for the leaderboard task.
   */
  WriteBackCache write_back_cache_ __attribute__((__unused__));

  /**
   * @brief A pointer to the log manager.
   *
   * Note: Please ignore this for P1.
   */
  LogManager *log_manager_ __attribute__((__unused__));

  /**
   * @brief Deallocates a page on disk. The caller should acquire the `BufferPoolManager` latch before calling this
   * function.
   *
   * Note: You should look at the documentation for `DeletePage` before using this method.
   *
   * @param page_id The page ID of the page to deallocate from disk.
   */
  void DeallocatePage(__attribute__((unused)) page_id_t page_id) {
    // Note that this is a no-op without a more complex data structure to track deallocated pages.
  }

  /** TODO(student): You may add additional private members and helper functions */
};
}  // namespace bustub
