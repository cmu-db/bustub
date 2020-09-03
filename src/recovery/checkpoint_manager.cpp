//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// checkpoint_manager.cpp
//
// Identification: src/recovery/checkpoint_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "recovery/checkpoint_manager.h"

namespace bustub {

void CheckpointManager::BeginCheckpoint() {
  // Block all the transactions and ensure that both the WAL and all dirty buffer pool pages are persisted to disk,
  // creating a consistent checkpoint. Do NOT allow transactions to resume at the end of this method, resume them
  // in CheckpointManager::EndCheckpoint() instead. This is for grading purposes.
  transaction_manager_->BlockAllTransactions();
  buffer_pool_manager_->FlushAllPages();
}

void CheckpointManager::EndCheckpoint() {
  // Allow transactions to resume, completing the checkpoint.
  transaction_manager_->ResumeTransactions();
}

}  // namespace bustub
