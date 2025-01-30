//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// transaction.h
//
// Identification: src/include/concurrency/transaction.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <fmt/format.h>
#include <atomic>
#include <bitset>
#include <cstddef>
#include <deque>
#include <limits>
#include <list>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <thread>  // NOLINT
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "common/config.h"
#include "common/logger.h"
#include "execution/expressions/abstract_expression.h"
#include "storage/page/page.h"
#include "storage/table/tuple.h"

namespace bustub {

class TransactionManager;

/**
 * Transaction State.
 */
enum class TransactionState { RUNNING = 0, TAINTED, COMMITTED = 100, ABORTED };

/**
 * Transaction isolation level. READ_UNCOMMITTED will NOT be used in project 3/4 as of Fall 2023.
 */
enum class IsolationLevel { READ_UNCOMMITTED, SNAPSHOT_ISOLATION, SERIALIZABLE };

class TableHeap;
class Catalog;
using table_oid_t = uint32_t;
using index_oid_t = uint32_t;

/** Represents a link to a previous version of this tuple */
struct UndoLink {
  /* Previous version can be found in which txn */
  txn_id_t prev_txn_{INVALID_TXN_ID};
  /* The log index of the previous version in `prev_txn_` */
  int prev_log_idx_{0};

  friend auto operator==(const UndoLink &a, const UndoLink &b) {
    return a.prev_txn_ == b.prev_txn_ && a.prev_log_idx_ == b.prev_log_idx_;
  }

  friend auto operator!=(const UndoLink &a, const UndoLink &b) { return !(a == b); }

  /* Checks if the undo link points to something. */
  auto IsValid() const -> bool { return prev_txn_ != INVALID_TXN_ID; }
};

struct UndoLog {
  /* Whether this log is a deletion marker */
  bool is_deleted_;
  /* The fields modified by this undo log */
  std::vector<bool> modified_fields_;
  /* The modified fields */
  Tuple tuple_;
  /* Timestamp of this undo log */
  timestamp_t ts_{INVALID_TS};
  /* Undo log prev version */
  UndoLink prev_version_{};
};

/**
 * Transaction tracks information related to a transaction.
 */
class Transaction {
 public:
  explicit Transaction(txn_id_t txn_id, IsolationLevel isolation_level = IsolationLevel::SNAPSHOT_ISOLATION)
      : isolation_level_(isolation_level), thread_id_(std::this_thread::get_id()), txn_id_(txn_id) {}

  ~Transaction() = default;

  DISALLOW_COPY(Transaction);

  /** @return the id of the thread running the transaction */
  inline auto GetThreadId() const -> std::thread::id { return thread_id_; }

  /** @return the id of this transaction */
  inline auto GetTransactionId() const -> txn_id_t { return txn_id_; }

  /** @return the id of this transaction, stripping the highest bit. NEVER use/store this value unless for debugging. */
  inline auto GetTransactionIdHumanReadable() const -> txn_id_t { return txn_id_ ^ TXN_START_ID; }

  /** @return the temporary timestamp of this transaction */
  inline auto GetTransactionTempTs() const -> timestamp_t { return txn_id_; }

  /** @return the isolation level of this transaction */
  inline auto GetIsolationLevel() const -> IsolationLevel { return isolation_level_; }

  /** @return the transaction state */
  inline auto GetTransactionState() const -> TransactionState { return state_; }

  /** @return the read ts */
  inline auto GetReadTs() const -> timestamp_t { return read_ts_; }

  /** @return the commit ts */
  inline auto GetCommitTs() const -> timestamp_t { return commit_ts_; }

  /** Modify an existing undo log. */
  inline auto ModifyUndoLog(int log_idx, UndoLog new_log) {
    std::scoped_lock<std::mutex> lck(latch_);
    undo_logs_[log_idx] = std::move(new_log);
  }

  /** @return the index of the undo log in this transaction */
  inline auto AppendUndoLog(UndoLog log) -> UndoLink {
    std::scoped_lock<std::mutex> lck(latch_);
    undo_logs_.emplace_back(std::move(log));
    return {txn_id_, static_cast<int>(undo_logs_.size() - 1)};
  }

  inline auto AppendWriteSet(table_oid_t t, RID rid) {
    std::scoped_lock<std::mutex> lck(latch_);
    write_set_[t].insert(rid);
  }

  inline auto GetWriteSets() -> const std::unordered_map<table_oid_t, std::unordered_set<RID>> & { return write_set_; }

  inline auto AppendScanPredicate(table_oid_t t, const AbstractExpressionRef &predicate) {
    std::scoped_lock<std::mutex> lck(latch_);
    scan_predicates_[t].emplace_back(predicate);
  }

  inline auto GetScanPredicates() -> const std::unordered_map<table_oid_t, std::vector<AbstractExpressionRef>> & {
    return scan_predicates_;
  }

  inline auto GetUndoLog(size_t log_id) -> UndoLog {
    std::scoped_lock<std::mutex> lck(latch_);
    return undo_logs_[log_id];
  }

  inline auto GetUndoLogNum() -> size_t {
    std::scoped_lock<std::mutex> lck(latch_);
    return undo_logs_.size();
  }

  /** Use this function in leaderboard benchmarks for online garbage collection. For stop-the-world GC, simply remove
   * the txn from the txn_map. */
  inline auto ClearUndoLog() -> size_t {
    std::scoped_lock<std::mutex> lck(latch_);
    return undo_logs_.size();
  }

  void SetTainted();

 private:
  friend class TransactionManager;

  // The below fields should be ONLY changed by txn manager (with the txn manager lock held).

  /** The state of this transaction. */
  std::atomic<TransactionState> state_{TransactionState::RUNNING};

  /** The read ts */
  std::atomic<timestamp_t> read_ts_{0};

  /** The commit ts */
  std::atomic<timestamp_t> commit_ts_{INVALID_TS};

  /** The latch for this transaction for accessing txn-level undo logs, protecting all fields below. */
  std::mutex latch_;

  /**
   * @brief Store undo logs. Other undo logs / table heap will store (txn_id, index) pairs, and therefore
   * you should only append to this vector or update things in-place without removing anything.
   */
  std::vector<UndoLog> undo_logs_;

  /** stores the RID of write tuples */
  std::unordered_map<table_oid_t, std::unordered_set<RID>> write_set_;
  /** store all scan predicates */
  std::unordered_map<table_oid_t, std::vector<AbstractExpressionRef>> scan_predicates_;

  // The below fields are set when a txn is created and will NEVER be changed.

  /** The isolation level of the transaction. */
  const IsolationLevel isolation_level_;

  /** The thread ID which the txn starts from.  */
  const std::thread::id thread_id_;

  /** The ID of this transaction. */
  const txn_id_t txn_id_;
};

}  // namespace bustub

template <>
struct fmt::formatter<bustub::IsolationLevel> : formatter<std::string_view> {
  // parse is inherited from formatter<string_view>.
  template <typename FormatContext>
  auto format(bustub::IsolationLevel x, FormatContext &ctx) const {
    using bustub::IsolationLevel;
    string_view name = "unknown";
    switch (x) {
      case IsolationLevel::READ_UNCOMMITTED:
        name = "READ_UNCOMMITTED";
        break;
      case IsolationLevel::SNAPSHOT_ISOLATION:
        name = "SNAPSHOT_ISOLATION";
        break;
      case IsolationLevel::SERIALIZABLE:
        name = "SERIALIZABLE";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};

template <>
struct fmt::formatter<bustub::TransactionState> : formatter<std::string_view> {
  // parse is inherited from formatter<string_view>.
  template <typename FormatContext>
  auto format(bustub::TransactionState x, FormatContext &ctx) const {
    using bustub::TransactionState;
    string_view name = "unknown";
    switch (x) {
      case TransactionState::RUNNING:
        name = "RUNNING";
        break;
      case TransactionState::ABORTED:
        name = "ABORTED";
        break;
      case TransactionState::COMMITTED:
        name = "COMMITTED";
        break;
      case TransactionState::TAINTED:
        name = "TAINTED";
        break;
    }
    return formatter<string_view>::format(name, ctx);
  }
};
