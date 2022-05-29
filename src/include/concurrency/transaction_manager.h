//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// transaction_manager.h
//
// Identification: src/include/concurrency/transaction_manager.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "common/config.h"
#include "concurrency/lock_manager.h"
#include "concurrency/transaction.h"
#include "recovery/log_manager.h"

namespace bustub {
class LockManager;

/**
 * TransactionManager keeps track of all the transactions running in the system.
 */
class TransactionManager {
 public:
  explicit TransactionManager(LockManager *lock_manager, LogManager *log_manager = nullptr)
      : lock_manager_(lock_manager), log_manager_(log_manager) {}

  ~TransactionManager() = default;

  /**
   * Begins a new transaction.
   * @param txn an optional transaction object to be initialized, otherwise a new transaction is created.
   * @param isolation_level an optional isolation level of the transaction.
   * @return an initialized transaction
   */
  auto Begin(Transaction *txn = nullptr, IsolationLevel isolation_level = IsolationLevel::REPEATABLE_READ)
      -> Transaction *;

  /**
   * Commits a transaction.
   * @param txn the transaction to commit
   */
  void Commit(Transaction *txn);

  /**
   * Aborts a transaction
   * @param txn the transaction to abort
   */
  void Abort(Transaction *txn);

  /**
   * Global list of running transactions
   */

  /** The transaction map is a global list of all the running transactions in the system. */
  static std::unordered_map<txn_id_t, Transaction *> txn_map;
  static std::shared_mutex txn_map_mutex;

  /**
   * Locates and returns the transaction with the given transaction ID.
   * @param txn_id the id of the transaction to be found, it must exist!
   * @return the transaction with the given transaction id
   */
  static auto GetTransaction(txn_id_t txn_id) -> Transaction * {
    TransactionManager::txn_map_mutex.lock_shared();
    assert(TransactionManager::txn_map.find(txn_id) != TransactionManager::txn_map.end());
    auto *res = TransactionManager::txn_map[txn_id];
    assert(res != nullptr);
    TransactionManager::txn_map_mutex.unlock_shared();
    return res;
  }

  /** Prevents all transactions from performing operations, used for checkpointing. */
  void BlockAllTransactions();

  /** Resumes all transactions, used for checkpointing. */
  void ResumeTransactions();

 private:
  /**
   * Releases all the locks held by the given transaction.
   * @param txn the transaction whose locks should be released
   */
  void ReleaseLocks(Transaction *txn) {
    std::unordered_set<RID> lock_set;
    for (auto item : *txn->GetExclusiveLockSet()) {
      lock_set.emplace(item);
    }
    for (auto item : *txn->GetSharedLockSet()) {
      lock_set.emplace(item);
    }
    for (auto locked_rid : lock_set) {
      lock_manager_->Unlock(txn, locked_rid);
    }
  }

  std::atomic<txn_id_t> next_txn_id_{0};
  LockManager *lock_manager_ __attribute__((__unused__));
  LogManager *log_manager_ __attribute__((__unused__));

  /** The global transaction latch is used for checkpointing. */
  ReaderWriterLatch global_txn_latch_;
};

}  // namespace bustub
