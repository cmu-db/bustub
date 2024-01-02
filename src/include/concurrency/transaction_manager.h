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

/// The first undo link in the version chain, that links table heap tuple to the undo log.
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

  /**
   * @brief Use this function before task 4.2. Update an undo link that links table heap tuple to the first undo log.
   * Before updating, `check` function will be called to ensure validity.
   */
  auto UpdateUndoLink(RID rid, std::optional<UndoLink> prev_link,
                      std::function<bool(std::optional<UndoLink>)> &&check = nullptr) -> bool;

  /**
   * @brief Use this function after task 4.2. Update an undo link that links table heap tuple to the first undo log.
   * Before updating, `check` function will be called to ensure validity.
   */
  auto UpdateVersionLink(RID rid, std::optional<VersionUndoLink> prev_version,
                         std::function<bool(std::optional<VersionUndoLink>)> &&check = nullptr) -> bool;

  /** @brief Get the first undo log of a table heap tuple. Use this before task 4.2 */
  auto GetUndoLink(RID rid) -> std::optional<UndoLink>;

  /** @brief Get the first undo log of a table heap tuple. Use this after task 4.2 */
  auto GetVersionLink(RID rid) -> std::optional<VersionUndoLink>;

  /** @brief Access the transaction undo log buffer and get the undo log. Return nullopt if the txn does not exist. Will
   * still throw an exception if the index is out of range. */
  auto GetUndoLogOptional(UndoLink link) -> std::optional<UndoLog>;

  /** @brief Access the transaction undo log buffer and get the undo log. Except when accessing the current txn buffer,
   * you should always call this function to get the undo log instead of manually retrieve the txn shared_ptr and access
   * the buffer. */
  auto GetUndoLog(UndoLink link) -> UndoLog;

  /** @brief Get the lowest read timestamp in the system. */
  auto GetWatermark() -> timestamp_t { return running_txns_.GetWatermark(); }

  /** @brief Stop-the-world garbage collection. Will be called only when all transactions are not accessing the table
   * heap. */
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
  /** Stores the previous version of each tuple in the table heap. Do not directly access this field. Use the helper
   * functions in `transaction_manager_impl.cpp`. */
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
  /** @brief Verify if a txn satisfies serializability. We will not test this function and you can change / remove it as
   * you want. */
  auto VerifyTxn(Transaction *txn) -> bool;
};

}  // namespace bustub
