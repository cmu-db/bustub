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
#include <string>
#include <thread>  // NOLINT
#include <unordered_set>

#include "common/config.h"
#include "common/logger.h"
#include "storage/page/page.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * Transaction states for 2PL:
 *
 *     _________________________
 *    |                         v
 * GROWING -> SHRINKING -> COMMITTED   ABORTED
 *    |__________|________________________^
 *
 * Transaction states for Non-2PL:
 *     __________
 *    |          v
 * GROWING  -> COMMITTED     ABORTED
 *    |_________________________^
 *
 **/
enum class TransactionState { GROWING, SHRINKING, COMMITTED, ABORTED };

/**
 * Transaction isolation level.
 */
enum class IsolationLevel { READ_UNCOMMITTED, REPEATABLE_READ, READ_COMMITTED };

/**
 * Type of write operation.
 */
enum class WType { INSERT = 0, DELETE, UPDATE };

class TableHeap;
class Catalog;
using table_oid_t = uint32_t;
using index_oid_t = uint32_t;

/**
 * WriteRecord tracks information related to a write.
 */
class TableWriteRecord {
 public:
  TableWriteRecord(RID rid, WType wtype, const Tuple &tuple, TableHeap *table)
      : rid_(rid), wtype_(wtype), tuple_(tuple), table_(table) {}

  RID rid_;
  WType wtype_;
  /** The tuple is only used for the update operation. */
  Tuple tuple_;
  /** The table heap specifies which table this write record is for. */
  TableHeap *table_;
};

/**
 * WriteRecord tracks information related to a write.
 */
class IndexWriteRecord {
 public:
  IndexWriteRecord(RID rid, table_oid_t table_oid, WType wtype, const Tuple &tuple, index_oid_t index_oid,
                   Catalog *catalog)
      : rid_(rid), table_oid_(table_oid), wtype_(wtype), tuple_(tuple), index_oid_(index_oid), catalog_(catalog) {}

  /** The rid is the value stored in the index. */
  RID rid_;
  /** Table oid. */
  table_oid_t table_oid_;
  /** Write type. */
  WType wtype_;
  /** The tuple is used to construct an index key. */
  Tuple tuple_;
  /** The old tuple is only used for the update operation. */
  Tuple old_tuple_;
  /** Each table has an index list, this is the identifier of an index into the list. */
  index_oid_t index_oid_;
  /** The catalog contains metadata required to locate index. */
  Catalog *catalog_;
};

/**
 * Reason to a transaction abortion
 */
enum class AbortReason {
  LOCK_ON_SHRINKING,
  UNLOCK_ON_SHRINKING,
  UPGRADE_CONFLICT,
  DEADLOCK,
  LOCKSHARED_ON_READ_UNCOMMITTED
};

/**
 * TransactionAbortException is thrown when state of a transaction is changed to ABORTED
 */
class TransactionAbortException : public std::exception {
  txn_id_t txn_id_;
  AbortReason abort_reason_;

 public:
  explicit TransactionAbortException(txn_id_t txn_id, AbortReason abort_reason)
      : txn_id_(txn_id), abort_reason_(abort_reason) {}
  txn_id_t GetTransactionId() { return txn_id_; }
  AbortReason GetAbortReason() { return abort_reason_; }
  std::string GetInfo() {
    switch (abort_reason_) {
      case AbortReason::LOCK_ON_SHRINKING:
        return "Transaction " + std::to_string(txn_id_) +
               " aborted because it can not take locks in the shrinking state\n";
      case AbortReason::UNLOCK_ON_SHRINKING:
        return "Transaction " + std::to_string(txn_id_) +
               " aborted because it can not excute unlock in the shrinking state\n";
      case AbortReason::UPGRADE_CONFLICT:
        return "Transaction " + std::to_string(txn_id_) +
               " aborted because another transaction is already waiting to upgrade its lock\n";
      case AbortReason::DEADLOCK:
        return "Transaction " + std::to_string(txn_id_) + " aborted on deadlock\n";
      case AbortReason::LOCKSHARED_ON_READ_UNCOMMITTED:
        return "Transaction " + std::to_string(txn_id_) + " aborted on lockshared on READ_UNCOMMITTED\n";
    }
    // Todo: Should fail with unreachable.
    return "";
  }
};

/**
 * Transaction tracks information related to a transaction.
 */
class Transaction {
 public:
  explicit Transaction(txn_id_t txn_id, IsolationLevel isolation_level = IsolationLevel::REPEATABLE_READ)
      : state_(TransactionState::GROWING),
        isolation_level_(isolation_level),
        thread_id_(std::this_thread::get_id()),
        txn_id_(txn_id),
        prev_lsn_(INVALID_LSN),
        shared_lock_set_{new std::unordered_set<RID>},
        exclusive_lock_set_{new std::unordered_set<RID>} {
    // Initialize the sets that will be tracked.
    table_write_set_ = std::make_shared<std::deque<TableWriteRecord>>();
    index_write_set_ = std::make_shared<std::deque<IndexWriteRecord>>();
    page_set_ = std::make_shared<std::deque<bustub::Page *>>();
    deleted_page_set_ = std::make_shared<std::unordered_set<page_id_t>>();
  }

  ~Transaction() = default;

  DISALLOW_COPY(Transaction);

  /** @return the id of the thread running the transaction */
  inline std::thread::id GetThreadId() const { return thread_id_; }

  /** @return the id of this transaction */
  inline txn_id_t GetTransactionId() const { return txn_id_; }

  /** @return the isolation level of this transaction */
  inline IsolationLevel GetIsolationLevel() const { return isolation_level_; }

  /** @return the list of table write records of this transaction */
  inline std::shared_ptr<std::deque<TableWriteRecord>> GetWriteSet() { return table_write_set_; }

  /** @return the list of index write records of this transaction */
  inline std::shared_ptr<std::deque<IndexWriteRecord>> GetIndexWriteSet() { return index_write_set_; }

  /** @return the page set */
  inline std::shared_ptr<std::deque<Page *>> GetPageSet() { return page_set_; }

  /**
   * Adds a tuple write record into the table write set.
   * @param write_record write record to be added
   */
  inline void AppendTableWriteRecord(const TableWriteRecord &write_record) {
    table_write_set_->push_back(write_record);
  }

  /**
   * Adds an index write record into the index write set.
   * @param write_record write record to be added
   */
  inline void AppendTableWriteRecord(const IndexWriteRecord &write_record) {
    index_write_set_->push_back(write_record);
  }

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

  /** @return true if rid is shared locked by this transaction */
  bool IsSharedLocked(const RID &rid) { return shared_lock_set_->find(rid) != shared_lock_set_->end(); }

  /** @return true if rid is exclusively locked by this transaction */
  bool IsExclusiveLocked(const RID &rid) { return exclusive_lock_set_->find(rid) != exclusive_lock_set_->end(); }

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
  /** The isolation level of the transaction. */
  IsolationLevel isolation_level_;
  /** The thread ID, used in single-threaded transactions. */
  std::thread::id thread_id_;
  /** The ID of this transaction. */
  txn_id_t txn_id_;

  /** The undo set of table tuples. */
  std::shared_ptr<std::deque<TableWriteRecord>> table_write_set_;
  /** The undo set of indexes. */
  std::shared_ptr<std::deque<IndexWriteRecord>> index_write_set_;
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
