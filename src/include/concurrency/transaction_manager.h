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
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "catalog/schema.h"
#include "common/config.h"
#include "concurrency/transaction.h"
#include "concurrency/watermark.h"
#include "recovery/log_manager.h"
#include "storage/table/tuple.h"

namespace bustub {

/**
 * TransactionManager keeps track of all the transactions running in the system.
 */
class TransactionManager {
 public:
  TransactionManager() = default;
  ~TransactionManager() = default;

  /**
   * Begins a new transaction.
   * @param txn an optional transaction object to be initialized, otherwise a new transaction is created.
   * @param isolation_level an optional isolation level of the transaction.
   * @return an initialized transaction
   */
  auto Begin(IsolationLevel isolation_level = IsolationLevel::SNAPSHOT_ISOLATION) -> Transaction * {
    std::unique_lock<std::shared_mutex> l(txn_map_mutex_);
    auto txn_id = next_txn_id_++;
    auto txn = std::make_unique<Transaction>(txn_id, isolation_level);
    auto *txn_ref = txn.get();
    txn_map_.insert(std::make_pair(txn_id, std::move(txn)));
    txn_ref->read_ts_ = last_commit_ts_;
    running_txns_.AddTxn(txn_ref->read_ts_);
    return txn_ref;
  }

  /**
   * Commits a transaction.
   * @param txn the transaction to commit, the txn will be managed by the txn manager so no need to delete it by
   * yourself
   */
  auto Commit(Transaction *txn) -> bool;

  /**
   * Aborts a transaction
   * @param txn the transaction to abort, the txn will be managed by the txn manager so no need to delete it by yourself
   */
  void Abort(Transaction *txn);

  auto UpdateVersionLink(RID rid, std::optional<UndoLink> prev_log,
                         std::function<bool(std::optional<UndoLink>)> &&check = nullptr) -> bool;

  auto GetVersionLink(RID rid) -> std::optional<UndoLink>;

  auto GetUndoLog(UndoLink link) -> UndoLog;

  auto GetWatermark() -> timestamp_t { return running_txns_.GetWatermark(); }

  void GarbageCollection();

  /** protects txn map */
  std::shared_mutex txn_map_mutex_;
  /** All transactions, running or committed */
  std::unordered_map<txn_id_t, std::shared_ptr<Transaction>> txn_map_;

  struct PageVersionInfo {
    /** protects the map */
    std::shared_mutex mutex_;
    /** Stores previous version info for all slots. Note: DO NOT use `[x]` to access it because
     * it will create new elements even if it does not exist. Use `find` instead.
     */
    std::unordered_map<slot_offset_t, UndoLink> prev_version_;
  };

  /** protects version info */
  std::shared_mutex version_info_mutex_;
  /** Stores the previous version of each tuple in the table heap. */
  std::unordered_map<page_id_t, std::shared_ptr<PageVersionInfo>> version_info_;

  /** Stores all the read_ts of running txns so as to facilitate garbage collection. */
  Watermark running_txns_{0};

  /** Only one txn is allowed to commit at a time */
  std::mutex commit_mutex_;
  /** The last committed timestamp. */
  std::atomic<timestamp_t> last_commit_ts_{0};

  /** Catalog */
  Catalog *catalog_;

 private:
  auto UpdateVersionLinkInternal(std::unique_lock<std::shared_mutex> lck, RID rid, std::optional<UndoLink> prev_log,
                                 std::function<bool(std::optional<UndoLink>)> &&check = nullptr) -> bool;

  std::atomic<txn_id_t> next_txn_id_{TXN_START_ID};
};

}  // namespace bustub
