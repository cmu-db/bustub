//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// transaction.h
//
// Identification: src/include/concurrency/transaction.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <thread>  // NOLINT
#include <unordered_set>

#include "common/config.h"
#include "common/logger.h"
#include "storage/page/page.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * Transaction states:
 *
 *     _________________________
 *    |                         v
 * GROWING -> SHRINKING -> COMMITTED   ABORTED
 *    |__________|________________________^
 *
 **/
enum class TransactionState { GROWING, SHRINKING, COMMITTED, ABORTED };

/**
 * Type of write operation.
 */
enum class WType { INSERT = 0, DELETE, UPDATE };

class TableHeap;

/**
 * WriteRecord tracks information related to a write.
 */
class WriteRecord {
 public:
  WriteRecord(RID rid, WType wtype, const Tuple &tuple, TableHeap *table)
      : rid_(rid), wtype_(wtype), tuple_(tuple), table_(table) {}

  RID rid_;
  WType wtype_;
  /** The tuple is only used for the update operation. */
  Tuple tuple_;
  /** The table heap specifies which table this write record is for. */
  TableHeap *table_;
};

/**
 * Transaction tracks information related to a transaction.
 */
class Transaction {
 public:
  explicit Transaction(txn_id_t txn_id)
      : state_(TransactionState::GROWING),
        thread_id_(std::this_thread::get_id()),
        txn_id_(txn_id),
        prev_lsn_(INVALID_LSN),
        shared_lock_set_{new std::unordered_set<RID>},
        exclusive_lock_set_{new std::unordered_set<RID>} {
    // Initialize the sets that will be tracked.
    write_set_ = std::make_shared<std::deque<WriteRecord>>();
    page_set_ = std::make_shared<std::deque<bustub::Page *>>();
    deleted_page_set_ = std::make_shared<std::unordered_set<page_id_t>>();
  }

  ~Transaction() = default;

  DISALLOW_COPY(Transaction);

  /** @return the id of the thread running the transaction */
  inline std::thread::id GetThreadId() const { return thread_id_; }

  /** @return the id of this transaction */
  inline txn_id_t GetTransactionId() const { return txn_id_; }

  /** @return the list of of write records of this transaction */
  inline std::shared_ptr<std::deque<WriteRecord>> GetWriteSet() { return write_set_; }

  /** @return the page set */
  inline std::shared_ptr<std::deque<Page *>> GetPageSet() { return page_set_; }

  /**
   * Adds a page into the page set.
   * @param page page to be added
   */
  inline void AddIntoPageSet(Page *page) { page_set_->push_back(page); }

  /** @return the deleted page set */
  inline std::shared_ptr<std::unordered_set<page_id_t>> GetDeletedPageSet() { return deleted_page_set_; }

  /**
   * Adds a page to the deleted page set.
   * @param page_id id of the page to be marked as deleted
   */
  inline void AddIntoDeletedPageSet(page_id_t page_id) { deleted_page_set_->insert(page_id); }

  /** @return the set of resources under a shared lock */
  inline std::shared_ptr<std::unordered_set<RID>> GetSharedLockSet() { return shared_lock_set_; }

  /** @return the set of resources under an exclusive lock */
  inline std::shared_ptr<std::unordered_set<RID>> GetExclusiveLockSet() { return exclusive_lock_set_; }

  /** @return the current state of the transaction */
  inline TransactionState GetState() { return state_; }

  /**
   * Set the state of the transaction.
   * @param state new state
   */
  inline void SetState(TransactionState state) { state_ = state; }

  /** @return the previous LSN */
  inline lsn_t GetPrevLSN() { return prev_lsn_; }

  /**
   * Set the previous LSN.
   * @param prev_lsn new previous lsn
   */
  inline void SetPrevLSN(lsn_t prev_lsn) { prev_lsn_ = prev_lsn; }

 private:
  /** The current transaction state. */
  TransactionState state_;
  /** The thread ID, used in single-threaded transactions. */
  std::thread::id thread_id_;
  /** The ID of this transaction. */
  txn_id_t txn_id_;

  /** The undo set of the transaction. */
  std::shared_ptr<std::deque<WriteRecord>> write_set_;
  /** The LSN of the last record written by the transaction. */
  lsn_t prev_lsn_;

  /** Concurrent index: the pages that were latched during index operation. */
  std::shared_ptr<std::deque<Page *>> page_set_;
  /** Concurrent index: the page IDs that were deleted during index operation.*/
  std::shared_ptr<std::unordered_set<page_id_t>> deleted_page_set_;

  /** LockManager: the set of shared-locked tuples held by this transaction. */
  std::shared_ptr<std::unordered_set<RID>> shared_lock_set_;
  /** LockManager: the set of exclusive-locked tuples held by this transaction. */
  std::shared_ptr<std::unordered_set<RID>> exclusive_lock_set_;
};

}  // namespace bustub
