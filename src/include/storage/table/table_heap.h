//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_heap.h
//
// Identification: src/include/storage/table/table_heap.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
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

/**
 * TableHeap represents a physical table on disk.
 * This is just a doubly-linked list of pages.
 */
class TableHeap {
  friend class TableIterator;

 public:
  ~TableHeap() = default;

  /**
   * Create a table heap without a transaction. (open table)
   * @param buffer_pool_manager the buffer pool manager
   * @param first_page_id the id of the first page
   */
  explicit TableHeap(BufferPoolManager *bpm);

  /**
   * Insert a tuple into the table. If the tuple is too large (>= page_size), return std::nullopt.
   * @param meta tuple meta
   * @param tuple tuple to insert
   * @return rid of the inserted tuple
   */
  auto InsertTuple(const TupleMeta &meta, const Tuple &tuple, LockManager *lock_mgr = nullptr,
                   Transaction *txn = nullptr, table_oid_t oid = 0) -> std::optional<RID>;

  /**
   * Insert a tuple into the table. If the tuple is too large (>= page_size), return false.
   * @param meta new tuple meta
   * @param[out] rid the rid of the inserted tuple
   */
  void UpdateTupleMeta(const TupleMeta &meta, RID rid);

  /**
   * Read a tuple from the table.
   * @param rid rid of the tuple to read
   * @return the meta and tuple
   */
  auto GetTuple(RID rid) -> std::pair<TupleMeta, Tuple>;

  /**
   * Read a tuple meta from the table. Note: if you want to get tuple and meta together, use `GetTuple` instead
   * to ensure atomicity.
   * @param rid rid of the tuple to read
   * @return the meta
   */
  auto GetTupleMeta(RID rid) -> TupleMeta;

  /** @return the iterator of this table, use this for project 3 */
  auto MakeIterator() -> TableIterator;

  /** @return the iterator of this table, use this for project 4 except updates */
  auto MakeEagerIterator() -> TableIterator;

  /** @return the id of the first page of this table */
  inline auto GetFirstPageId() const -> page_id_t { return first_page_id_; }

  /**
   * Update a tuple in place. SHOULD NOT BE USED UNLESS YOU WANT TO OPTIMIZE FOR PROJECT 4.
   * @param meta new tuple meta
   * @param tuple  new tuple
   * @param[out] rid the rid of the tuple to be updated
   */
  auto UpdateTupleInPlace(const TupleMeta &meta, const Tuple &tuple, RID rid,
                          std::function<bool(const TupleMeta &meta, const Tuple &table, RID rid)> &&check = nullptr)
      -> bool;

  /** For binder tests */
  static auto CreateEmptyHeap(bool create_table_heap = false) -> std::unique_ptr<TableHeap> {
    // The input parameter should be false in order to generate a empty heap
    assert(!create_table_heap);
    return std::unique_ptr<TableHeap>(new TableHeap(create_table_heap));
  }

 private:
  /** Used for binder tests */
  explicit TableHeap(bool create_table_heap = false);

  BufferPoolManager *bpm_;
  page_id_t first_page_id_{INVALID_PAGE_ID};

  std::mutex latch_;
  page_id_t last_page_id_{INVALID_PAGE_ID}; /* protected by latch_ */
};

}  // namespace bustub
