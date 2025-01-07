//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lock_manager.cpp
//
// Identification: src/concurrency/lock_manager.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "concurrency/lock_manager.h"

#include "common/config.h"
#include "concurrency/transaction.h"
#include "concurrency/transaction_manager.h"

namespace bustub {

/**
 * Acquire a lock on table_oid_t in the given lock_mode.
 * If the transaction already holds a lock on the table, upgrade the lock
 * to the specified lock_mode (if possible).
 *
 * This method should abort the transaction and throw a
 * TransactionAbortException under certain circumstances.
 * See [LOCK_NOTE] in header file.
 *
 * @param txn the transaction requesting the lock upgrade
 * @param lock_mode the lock mode for the requested lock
 * @param oid the table_oid_t of the table to be locked in lock_mode
 * @return true if the upgrade is successful, false otherwise
 */
auto LockManager::LockTable(Transaction *txn, LockMode lock_mode, const table_oid_t &oid) -> bool { return true; }

/**
 * Release the lock held on a table by the transaction.
 *
 * This method should abort the transaction and throw a
 * TransactionAbortException under certain circumstances.
 * See [UNLOCK_NOTE] in header file.
 *
 * @param txn the transaction releasing the lock
 * @param oid the table_oid_t of the table to be unlocked
 * @return true if the unlock is successful, false otherwise
 */
auto LockManager::UnlockTable(Transaction *txn, const table_oid_t &oid) -> bool { return true; }

/**
 * Acquire a lock on rid in the given lock_mode.
 * If the transaction already holds a lock on the row, upgrade the lock
 * to the specified lock_mode (if possible).
 *
 * This method should abort the transaction and throw a
 * TransactionAbortException under certain circumstances.
 * See [LOCK_NOTE] in header file.
 *
 * @param txn the transaction requesting the lock upgrade
 * @param lock_mode the lock mode for the requested lock
 * @param oid the table_oid_t of the table the row belongs to
 * @param rid the RID of the row to be locked
 * @return true if the upgrade is successful, false otherwise
 */
auto LockManager::LockRow(Transaction *txn, LockMode lock_mode, const table_oid_t &oid, const RID &rid) -> bool {
  return true;
}

/**
 * Release the lock held on a row by the transaction.
 *
 * This method should abort the transaction and throw a
 * TransactionAbortException under certain circumstances.
 * See [UNLOCK_NOTE] in header file.
 *
 * @param txn the transaction releasing the lock
 * @param rid the RID that is locked by the transaction
 * @param oid the table_oid_t of the table the row belongs to
 * @param rid the RID of the row to be unlocked
 * @param force unlock the tuple regardless of isolation level, not changing the transaction state
 * @return true if the unlock is successful, false otherwise
 */
auto LockManager::UnlockRow(Transaction *txn, const table_oid_t &oid, const RID &rid, bool force) -> bool {
  return true;
}

void LockManager::UnlockAll() {
  // You probably want to unlock all table and txn locks here.
}

/**
 * Adds an edge from t1 -> t2 from waits for graph.
 * @param t1 transaction waiting for a lock
 * @param t2 transaction being waited for
 */
void LockManager::AddEdge(txn_id_t t1, txn_id_t t2) {}

/**
 * Removes an edge from t1 -> t2 from waits for graph.
 * @param t1 transaction waiting for a lock
 * @param t2 transaction being waited for
 */
void LockManager::RemoveEdge(txn_id_t t1, txn_id_t t2) {}

/**
 * Checks if the graph has a cycle, returning the newest transaction ID in the cycle if so.
 * @param[out] txn_id if the graph has a cycle, will contain the newest transaction ID
 * @return false if the graph has no cycle, otherwise stores the newest transaction ID in the cycle to txn_id
 */
auto LockManager::HasCycle(txn_id_t *txn_id) -> bool { return false; }

/**
 * @return all edges in current waits_for graph
 */
auto LockManager::GetEdgeList() -> std::vector<std::pair<txn_id_t, txn_id_t>> {
  std::vector<std::pair<txn_id_t, txn_id_t>> edges(0);
  return edges;
}

/**
 * Runs cycle detection in the background.
 */
void LockManager::RunCycleDetection() {
  while (enable_cycle_detection_) {
    std::this_thread::sleep_for(cycle_detection_interval);
    {  // TODO(students): detect deadlock
    }
  }
}

}  // namespace bustub
