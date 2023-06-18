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
#include "concurrency/transaction_manager.h"

#include <utility>
#include <vector>

namespace bustub {

bool LockManager::LockPrepare(Transaction *txn, const RID &rid) {
  if (txn->GetState() == TransactionState::SHRINKING){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_ON_SHRINKING);
    return false;
  }

  if (lock_table_.find(rid) == lock_table_.end()){
    lock_table_.emplace(std::piecewise_construct,
                        std::forward_as_tuple(rid),
                        std::forward_as_tuple());
  }
  return true;
}

std::list<LockManager::LockRequest>::iterator LockManager::GetIterator(std::list<LockRequest> &request_queue, txn_id_t txn_id) {
  for (auto iter = request_queue.begin(); iter != request_queue.end(); ++iter){
    if (iter->txn_id_ == txn_id){
      return iter;
    }
  }
  return request_queue.end();
}

void LockManager::check_aborted(Transaction *txn, LockRequestQueue* request_queue) {
  if (txn->GetState() == TransactionState::ABORTED){
    auto iter = GetIterator(request_queue->request_queue_, txn->GetTransactionId());
    request_queue->request_queue_.erase(iter);
    throw TransactionAbortException(txn->GetTransactionId(),
                                    AbortReason::DEADLOCK);
  }
}

bool LockManager::LockShared(Transaction *txn, const RID &rid) {
  std::unique_lock<std::mutex> lock(latch_);

  if (txn->GetIsolationLevel() == IsolationLevel::READ_UNCOMMITTED){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCKSHARED_ON_READ_UNCOMMITTED);
    return false;
  }
  if (!LockPrepare(txn, rid)){
    return false;
  }

  LockRequestQueue* request_queue = &lock_table_.find(rid)->second;
  request_queue->request_queue_.emplace_back(txn->GetTransactionId(),
                                             LockMode::SHARED);

  if (request_queue->is_writing_){
    request_queue->cv_.wait(lock,
                            [request_queue, txn]()
                                ->bool{return txn->GetState() == TransactionState::ABORTED ||
                                                 !request_queue->is_writing_;}
                            );
  }

  check_aborted(txn, request_queue);

  txn->GetSharedLockSet()->emplace(rid);
  request_queue->sharing_count_++;
  auto iter = GetIterator(request_queue->request_queue_, txn->GetTransactionId());
  iter->granted_ = true;

  return true;
}

bool LockManager::LockExclusive(Transaction *txn, const RID &rid) {
  std::unique_lock<std::mutex> lock(latch_);

  if (!LockPrepare(txn, rid)){
    return false;
  }

  LockRequestQueue* request_queue = &lock_table_.find(rid)->second;
  request_queue->request_queue_.emplace_back(txn->GetTransactionId(),
                                             LockMode::EXCLUSIVE);

  if (request_queue->is_writing_ || request_queue->sharing_count_ > 0){
    request_queue->cv_.wait(lock,
                            [request_queue, txn]()
                                ->bool{ return txn->GetState() == TransactionState::ABORTED ||
                                                 (!request_queue->is_writing_ && request_queue->sharing_count_ == 0);}
                            );
  }

  check_aborted(txn, request_queue);

  txn->GetExclusiveLockSet()->emplace(rid);
  request_queue->is_writing_ = true;
  auto iter = GetIterator(request_queue->request_queue_, txn->GetTransactionId());
  iter->granted_ = true;

  return true;
}

bool LockManager::LockUpgrade(Transaction *txn, const RID &rid) {
  std::unique_lock<std::mutex> lock(latch_);

  if (txn->GetState() == TransactionState::SHRINKING){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::LOCK_ON_SHRINKING);
    return false;
  }

  LockRequestQueue* request_queue = &lock_table_.find(rid)->second;

  if (request_queue->upgrading_){
    txn->SetState(TransactionState::ABORTED);
    throw TransactionAbortException(txn->GetTransactionId(), AbortReason::UPGRADE_CONFLICT);
    return false;
  }

  txn->GetSharedLockSet()->erase(rid);
  request_queue->sharing_count_--;
  auto iter = GetIterator(request_queue->request_queue_, txn->GetTransactionId());
  iter->lock_mode_ = LockMode::EXCLUSIVE;
  iter->granted_ = false;

  if (request_queue->is_writing_ || request_queue->sharing_count_ > 0){
    request_queue->upgrading_ = true;
    request_queue->cv_.wait(lock,
                            [request_queue, txn]()
                                ->bool{return txn->GetState() == TransactionState::ABORTED ||
                                                 (!request_queue->is_writing_ && request_queue->sharing_count_ == 0);}
                            );
  }

  check_aborted(txn, request_queue);

  txn->GetExclusiveLockSet()->emplace(rid);
  request_queue->upgrading_ = false;
  request_queue->is_writing_ = true;
  iter = GetIterator(request_queue->request_queue_, txn->GetTransactionId());
  iter->granted_ = true;

  return true;
}

bool LockManager::Unlock(Transaction *txn, const RID &rid) {
  std::unique_lock<std::mutex> lock(latch_);

  LockRequestQueue* request_queue = &lock_table_.find(rid)->second;

  txn->GetSharedLockSet()->erase(rid);
  txn->GetExclusiveLockSet()->erase(rid);

  auto iter = GetIterator(request_queue->request_queue_, txn->GetTransactionId());
  LockMode mode = iter->lock_mode_;
  request_queue->request_queue_.erase(iter);

  if (!(mode == LockMode::SHARED && txn->GetIsolationLevel() == IsolationLevel::READ_COMMITTED)
      && txn->GetState() == TransactionState::GROWING){
    txn->SetState(TransactionState::SHRINKING);
  }

  if (mode == LockMode::SHARED){
    if (--request_queue->sharing_count_ == 0){
      request_queue->cv_.notify_all();
    }
  }
  else{
    request_queue->is_writing_ = false;
    request_queue->cv_.notify_all();
  }

  return true;
}

void LockManager::AddEdge(txn_id_t t1, txn_id_t t2) {
  waits_for_[t1].push_back(t2);
}

void LockManager::RemoveEdge(txn_id_t t1, txn_id_t t2) {
  auto iter = std::find(waits_for_[t1].begin(), waits_for_[t1].end(), t1);
  if (iter != waits_for_[t1].end()){
    waits_for_[t1].erase(iter);
  }
}

void LockManager::DeleteNode(txn_id_t txn_id) {
  waits_for_.erase(txn_id);
  Transaction* txn = TransactionManager::GetTransaction(txn_id);

  for (RID const &lock_rid : *(txn->GetSharedLockSet())){
    for (LockRequest const &lock_request : lock_table_[lock_rid].request_queue_){
      if (!lock_request.granted_){
        RemoveEdge(lock_request.txn_id_, txn_id);
      }
    }
  }

  for (RID const &lock_rid : *(txn->GetExclusiveLockSet())){
    for (LockRequest const &lock_request : lock_table_[lock_rid].request_queue_){
      if (!lock_request.granted_){
        RemoveEdge(lock_request.txn_id_, txn_id);
      }
    }
  }

}

bool LockManager::dfs(txn_id_t txn_id) {
  if (safe_set_.find(txn_id) != safe_set_.end()){
    return false;
  }
  active_set_.insert(txn_id);

  std::vector<txn_id_t> &next_node_vector = waits_for_[txn_id];
  std::sort(next_node_vector.begin(), next_node_vector.end());
  for (txn_id_t const next_node : next_node_vector){
    if (active_set_.find(next_node) != active_set_.end()){
      return true;
    }
    if (dfs(next_node)){
      return true;
    }
  }

  active_set_.erase(txn_id);
  safe_set_.insert(txn_id);
  return false;
}

bool LockManager::HasCycle(txn_id_t *txn_id) {
  for (auto const &start_txn_id : txn_set_){
    if (dfs(start_txn_id)){
      *txn_id = *active_set_.begin();
      for (auto const &active_txn_id : active_set_){
        *txn_id = std::max(*txn_id, active_txn_id);
      }
      active_set_.clear();
      return true;
    }

    active_set_.clear();
    txn_set_.erase(start_txn_id);
  }
  return false;
}

std::vector<std::pair<txn_id_t, txn_id_t>> LockManager::GetEdgeList() {
  std::vector<std::pair<txn_id_t, txn_id_t>> result;
  for (auto const &pair : waits_for_){
    auto t1 = pair.first;
    for (auto const &t2 : pair.second){
      result.emplace_back(t1, t2);
    }
  }
  return result;
}

// Dyy : In the handout they say 'throw a exception', but throw a exception
//       out a thread will crash the whole process! So I don't throw exception
//       in this function, but in Lock function
void LockManager::RunCycleDetection() {
  while (enable_cycle_detection_) {
    std::this_thread::sleep_for(cycle_detection_interval);
    {
      // TODO(student): remove the continue and add your cycle detection and abort code here
      // First build the entire graph
      std::unique_lock<std::mutex> l(latch_);
      for (auto const &pair : lock_table_){
        for (auto const &lock_request_waiting : pair.second.request_queue_){
          txn_set_.insert(lock_request_waiting.txn_id_);
          // If it is waiting, add a edge to each lock-granted txn
          if (lock_request_waiting.granted_) { continue; }
          require_record_[lock_request_waiting.txn_id_] = pair.first;
          for (auto const &lock_request_granted : pair.second.request_queue_){
            if (!lock_request_granted.granted_) { continue; }
            AddEdge(lock_request_waiting.txn_id_, lock_request_granted.txn_id_);
          }
        }
      }

      // Break every cycle
      txn_id_t txn_id;
      while (HasCycle(&txn_id)){
        Transaction* txn = TransactionManager::GetTransaction(txn_id);
        txn->SetState(TransactionState::ABORTED);
        DeleteNode(txn_id);
        lock_table_[require_record_[txn_id]].cv_.notify_all();
      }

      // clear internal data, next time we will construct them on-the-fly!
      waits_for_.clear();
      safe_set_.clear();
      txn_set_.clear();
      require_record_.clear();

    }
  }
}

}  // namespace bustub
