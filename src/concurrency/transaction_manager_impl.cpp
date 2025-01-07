//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// transaction_manager_impl.cpp
//
// Identification: src/concurrency/transaction_manager_impl.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

// DO NOT CHANGE THIS FILE, this file will not be included in the autograder.

#include <exception>
#include <memory>
#include <mutex>  // NOLINT
#include <optional>
#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>

#include "catalog/catalog.h"
#include "catalog/column.h"
#include "catalog/schema.h"
#include "common/config.h"
#include "common/exception.h"
#include "common/macros.h"
#include "concurrency/transaction.h"
#include "concurrency/transaction_manager.h"
#include "execution/execution_common.h"
#include "storage/table/table_heap.h"
#include "storage/table/tuple.h"
#include "type/type_id.h"
#include "type/value.h"
#include "type/value_factory.h"

namespace bustub {

/**
 * @brief Update an undo link that links table heap tuple to the first undo log.
 * Before updating, `check` function will be called to ensure validity.
 */
auto TransactionManager::UpdateUndoLink(RID rid, std::optional<UndoLink> prev_link,
                                        std::function<bool(std::optional<UndoLink>)> &&check) -> bool {
  std::unique_lock<std::shared_mutex> lck(version_info_mutex_);
  std::shared_ptr<PageVersionInfo> pg_ver_info = nullptr;
  auto iter = version_info_.find(rid.GetPageId());
  if (iter == version_info_.end()) {
    pg_ver_info = std::make_shared<PageVersionInfo>();
    version_info_[rid.GetPageId()] = pg_ver_info;
  } else {
    pg_ver_info = iter->second;
  }
  std::unique_lock<std::shared_mutex> lck2(pg_ver_info->mutex_);
  lck.unlock();
  auto iter2 = pg_ver_info->prev_link_.find(rid.GetSlotNum());
  if (iter2 == pg_ver_info->prev_link_.end()) {
    if (check != nullptr && !check(std::nullopt)) {
      return false;
    }
  } else {
    if (check != nullptr && !check(iter2->second)) {
      return false;
    }
  }
  if (prev_link.has_value()) {
    pg_ver_info->prev_link_[rid.GetSlotNum()] = *prev_link;
  } else {
    pg_ver_info->prev_link_.erase(rid.GetSlotNum());
  }
  return true;
}

/** @brief Get the first undo log of a table heap tuple. */
auto TransactionManager::GetUndoLink(RID rid) -> std::optional<UndoLink> {
  std::shared_lock<std::shared_mutex> lck(version_info_mutex_);
  auto iter = version_info_.find(rid.GetPageId());
  if (iter == version_info_.end()) {
    return std::nullopt;
  }
  std::shared_ptr<PageVersionInfo> pg_ver_info = iter->second;
  std::unique_lock<std::shared_mutex> lck2(pg_ver_info->mutex_);
  lck.unlock();
  auto iter2 = pg_ver_info->prev_link_.find(rid.GetSlotNum());
  if (iter2 == pg_ver_info->prev_link_.end()) {
    return std::nullopt;
  }
  return std::make_optional(iter2->second);
}

/** @brief Access the transaction undo log buffer and get the undo log. Return nullopt if the txn does not exist. Will
 * still throw an exception if the index is out of range. */
auto TransactionManager::GetUndoLogOptional(UndoLink link) -> std::optional<UndoLog> {
  std::shared_lock<std::shared_mutex> lck(txn_map_mutex_);
  auto iter = txn_map_.find(link.prev_txn_);
  if (iter == txn_map_.end()) {
    return std::nullopt;
  }
  auto txn = iter->second;
  lck.unlock();
  return txn->GetUndoLog(link.prev_log_idx_);
}

/** @brief Access the transaction undo log buffer and get the undo log. Except when accessing the current txn buffer,
 * you should always call this function to get the undo log instead of manually retrieve the txn shared_ptr and access
 * the buffer. */
auto TransactionManager::GetUndoLog(UndoLink link) -> UndoLog {
  auto undo_log = GetUndoLogOptional(link);
  if (undo_log.has_value()) {
    return *undo_log;
  }
  throw Exception("undo log not exist");
}

void Transaction::SetTainted() {
  auto state = state_.load();
  if (state == TransactionState::RUNNING) {
    state_.store(TransactionState::TAINTED);
    return;
  }
  fmt::println(stderr, "transaction not in running state: {}", state);
  std::terminate();
}

/**
 * @brief Update the tuple and its undo link in the table heap atomically.
 */
auto UpdateTupleAndUndoLink(
    TransactionManager *txn_mgr, RID rid, std::optional<UndoLink> undo_link, TableHeap *table_heap, Transaction *txn,
    const TupleMeta &meta, const Tuple &tuple,
    std::function<bool(const TupleMeta &meta, const Tuple &tuple, RID rid, std::optional<UndoLink>)> &&check) -> bool {
  auto page_write_guard = table_heap->AcquireTablePageWriteLock(rid);
  auto page = page_write_guard.AsMut<TablePage>();

  auto [base_meta, base_tuple] = page->GetTuple(rid);
  if (check != nullptr && !check(base_meta, base_tuple, rid, undo_link)) {
    return false;
  }

  // Update tuple and tupleMeta if pass in tuple and meta are different
  if (meta != base_meta || !IsTupleContentEqual(tuple, base_tuple)) {
    table_heap->UpdateTupleInPlaceWithLockAcquired(meta, tuple, rid, page);
  }

  txn_mgr->UpdateUndoLink(rid, undo_link);

  return true;
}

/**
 * @brief Get the tuple and its undo link in the table heap atomically.
 */
auto GetTupleAndUndoLink(TransactionManager *txn_mgr, TableHeap *table_heap, RID rid)
    -> std::tuple<TupleMeta, Tuple, std::optional<UndoLink>> {
  auto page_read_guard = table_heap->AcquireTablePageReadLock(rid);
  auto page = page_read_guard.As<TablePage>();
  auto [meta, tuple] = page->GetTuple(rid);

  auto undo_link = txn_mgr->GetUndoLink(rid);
  return std::make_tuple(meta, tuple, undo_link);
}

}  // namespace bustub
