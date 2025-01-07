//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_heap.h
//
// Identification: src/include/storage/table/table_heap.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <utility>

#include "buffer/buffer_pool_manager.h"
#include "common/config.h"
#include "concurrency/lock_manager.h"
#include "concurrency/transaction.h"
#include "recovery/log_manager.h"
#include "storage/page/page_guard.h"
#include "storage/page/table_page.h"
#include "storage/table/table_iterator.h"
#include "storage/table/tuple.h"

namespace bustub {

class TablePage;

/**
 * TableHeap represents a physical table on disk.
 * This is just a doubly-linked list of pages.
 */
class TableHeap {
  friend class TableIterator;

 public:
  ~TableHeap() = default;

  explicit TableHeap(BufferPoolManager *bpm);

  auto InsertTuple(const TupleMeta &meta, const Tuple &tuple, LockManager *lock_mgr = nullptr,
                   Transaction *txn = nullptr, table_oid_t oid = 0) -> std::optional<RID>;

  void UpdateTupleMeta(const TupleMeta &meta, RID rid);

  auto GetTuple(RID rid) -> std::pair<TupleMeta, Tuple>;

  auto GetTupleMeta(RID rid) -> TupleMeta;

  auto MakeIterator() -> TableIterator;

  auto MakeEagerIterator() -> TableIterator;

  /** @return the id of the first page of this table */
  inline auto GetFirstPageId() const -> page_id_t { return first_page_id_; }

  auto UpdateTupleInPlace(const TupleMeta &meta, const Tuple &tuple, RID rid,
                          std::function<bool(const TupleMeta &meta, const Tuple &table, RID rid)> &&check = nullptr)
      -> bool;

  /** For binder tests */
  static auto CreateEmptyHeap(bool create_table_heap = false) -> std::unique_ptr<TableHeap> {
    // The input parameter should be false in order to generate a empty heap
    assert(!create_table_heap);
    return std::unique_ptr<TableHeap>(new TableHeap(create_table_heap));
  }

  // The below functions are useful only when you want to implement abort in a way that removes an undo log from the
  // version chain. DO NOT USE THEM if you are unsure what they are supposed to do.
  //
  // And if you decide to use the below functions, DO NOT use the normal ones like `GetTuple`. Having two read locks
  // on the same thing in one thread might cause deadlocks.

  auto AcquireTablePageReadLock(RID rid) -> ReadPageGuard;

  auto AcquireTablePageWriteLock(RID rid) -> WritePageGuard;

  void UpdateTupleInPlaceWithLockAcquired(const TupleMeta &meta, const Tuple &tuple, RID rid, TablePage *page);

  auto GetTupleWithLockAcquired(RID rid, const TablePage *page) -> std::pair<TupleMeta, Tuple>;

  auto GetTupleMetaWithLockAcquired(RID rid, const TablePage *page) -> TupleMeta;

 private:
  /** Used for binder tests */
  explicit TableHeap(bool create_table_heap = false);

  BufferPoolManager *bpm_;
  page_id_t first_page_id_{INVALID_PAGE_ID};

  std::mutex latch_;
  page_id_t last_page_id_{INVALID_PAGE_ID}; /* protected by latch_ */
};

}  // namespace bustub
