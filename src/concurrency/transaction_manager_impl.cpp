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

auto TransactionManager::UpdateUndoLink(RID rid, std::optional<UndoLink> prev_link,
                                        std::function<bool(std::optional<UndoLink>)> &&check) -> bool {
  std::function<bool(std::optional<VersionUndoLink>)> wrapper_func =
      [check](std::optional<VersionUndoLink> link) -> bool {
    if (link.has_value()) {
      return check(link->prev_);
    }
    return check(std::nullopt);
  };
  return UpdateVersionLink(rid, prev_link.has_value() ? std::make_optional(VersionUndoLink{*prev_link}) : std::nullopt,
                           check != nullptr ? wrapper_func : nullptr);
}

auto TransactionManager::UpdateVersionLink(RID rid, std::optional<VersionUndoLink> prev_version,
                                           std::function<bool(std::optional<VersionUndoLink>)> &&check) -> bool {
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
  auto iter2 = pg_ver_info->prev_version_.find(rid.GetSlotNum());
  if (iter2 == pg_ver_info->prev_version_.end()) {
    if (check != nullptr && !check(std::nullopt)) {
      return false;
    }
  } else {
    if (check != nullptr && !check(iter2->second)) {
      return false;
    }
  }
  if (prev_version.has_value()) {
    pg_ver_info->prev_version_[rid.GetSlotNum()] = *prev_version;
  } else {
    pg_ver_info->prev_version_.erase(rid.GetSlotNum());
  }
  return true;
}

auto TransactionManager::GetVersionLink(RID rid) -> std::optional<VersionUndoLink> {
  std::shared_lock<std::shared_mutex> lck(version_info_mutex_);
  auto iter = version_info_.find(rid.GetPageId());
  if (iter == version_info_.end()) {
    return std::nullopt;
  }
  std::shared_ptr<PageVersionInfo> pg_ver_info = iter->second;
  std::unique_lock<std::shared_mutex> lck2(pg_ver_info->mutex_);
  lck.unlock();
  auto iter2 = pg_ver_info->prev_version_.find(rid.GetSlotNum());
  if (iter2 == pg_ver_info->prev_version_.end()) {
    return std::nullopt;
  }
  return std::make_optional(iter2->second);
}

auto TransactionManager::GetUndoLink(RID rid) -> std::optional<UndoLink> {
  auto version_link = GetVersionLink(rid);
  if (version_link.has_value()) {
    return version_link->prev_;
  }
  return std::nullopt;
}

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

}  // namespace bustub
