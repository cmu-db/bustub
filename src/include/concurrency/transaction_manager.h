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
#include <mutex>  // NOLINT
#include <optional>
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

struct VersionUndoLink {
  /** The next version in the version chain. */
  UndoLink prev_;
  /** Whether a transaction is modifying the version link. Fall 2023: you do not need to read / write this field until
   * task 4.2. */
  bool in_progress_{false};

  friend auto operator==(const VersionUndoLink &a, const VersionUndoLink &b) {
    return a.prev_ == b.prev_ && a.in_progress_ == b.in_progress_;
  }

  friend auto operator!=(const VersionUndoLink &a, const VersionUndoLink &b) { return !(a == b); }

  inline static auto FromOptionalUndoLink(std::optional<UndoLink> undo_link) -> std::optional<VersionUndoLink> {
    if (undo_link.has_value()) {
      return VersionUndoLink{*undo_link};
    }
    return std::nullopt;
  }
};

/**
 * TransactionManager keeps track of all the transactions running in the system.
 */
class TransactionManager {
 public:
  TransactionManager() = default;
  ~TransactionManager() = default;

  /**
   * Begins a new transaction.
   * @param isolation_level an optional isolation level of the transaction.
   * @return an initialized transaction
   */
  auto Begin(IsolationLevel isolation_level = IsolationLevel::SNAPSHOT_ISOLATION) -> Transaction *;

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

  auto UpdateVersionLink(RID rid, std::optional<VersionUndoLink> prev_version,
                         std::function<bool(std::optional<VersionUndoLink>)> &&check = nullptr) -> bool;

  /** The same as `GetVersionLink`, except that we extracted the undo link field out. */
  auto GetUndoLink(RID rid) -> std::optional<UndoLink>;

  /** You only need this starting task 4.2 */
  auto GetVersionLink(RID rid) -> std::optional<VersionUndoLink>;

  auto GetUndoLogOptional(UndoLink link) -> std::optional<UndoLog>;

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
    std::unordered_map<slot_offset_t, VersionUndoLink> prev_version_;
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

  std::atomic<txn_id_t> next_txn_id_{TXN_START_ID};

 private:
  auto VerifyTxn(Transaction *txn) -> bool;
};

}  // namespace bustub
