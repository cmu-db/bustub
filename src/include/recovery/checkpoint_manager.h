//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// checkpoint_manager.h
//
// Identification: src/include/recovery/checkpoint_manager.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "buffer/buffer_pool_manager.h"
#include "concurrency/transaction_manager.h"
#include "recovery/log_manager.h"

namespace bustub {

/**
 * CheckpointManager creates consistent checkpoints by blocking all other transactions temporarily.
 */
class CheckpointManager {
 public:
  CheckpointManager(TransactionManager *transaction_manager, LogManager *log_manager,
                    BufferPoolManager *buffer_pool_manager)
      : transaction_manager_(transaction_manager),
        log_manager_(log_manager),
        buffer_pool_manager_(buffer_pool_manager) {}

  ~CheckpointManager() = default;

  void BeginCheckpoint();
  void EndCheckpoint();

 private:
  TransactionManager *transaction_manager_ __attribute__((__unused__));
  LogManager *log_manager_ __attribute__((__unused__));
  BufferPoolManager *buffer_pool_manager_ __attribute__((__unused__));
};

}  // namespace bustub
