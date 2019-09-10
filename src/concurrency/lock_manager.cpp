//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lock_manager.cpp
//
// Identification: src/concurrency/lock_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "concurrency/lock_manager.h"

#include <utility>
#include <vector>

namespace bustub {

bool LockManager::LockShared(Transaction *txn, const RID &rid) { return false; }

bool LockManager::LockExclusive(Transaction *txn, const RID &rid) { return false; }

bool LockManager::LockUpgrade(Transaction *txn, const RID &rid) { return false; }

bool LockManager::Unlock(Transaction *txn, const RID &rid) { return false; }

void LockManager::AddEdge(txn_id_t t1, txn_id_t t2) { assert(Detection()); }

void LockManager::RemoveEdge(txn_id_t t1, txn_id_t t2) { assert(Detection()); }

bool LockManager::HasCycle(txn_id_t *txn_id) {
  BUSTUB_ASSERT(Detection(), "Detection should be enabled!");
  return false;
}

std::vector<std::pair<txn_id_t, txn_id_t>> LockManager::GetEdgeList() {
  BUSTUB_ASSERT(Detection(), "Detection should be enabled!");
  return {};
}

void LockManager::RunCycleDetection() {
  BUSTUB_ASSERT(Detection(), "Detection should be enabled!");
  while (enable_cycle_detection_) {
    std::this_thread::sleep_for(CYCLE_DETECTION_INTERVAL);
    {
      std::unique_lock<std::mutex> l(latch_);
      // TODO(student): remove the continue and add your cycle detection and abort code here
      continue;
    }
  }
}

}  // namespace bustub
