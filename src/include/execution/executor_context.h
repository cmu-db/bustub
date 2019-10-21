//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// executor_context.h
//
// Identification: src/include/execution/executor_context.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <unordered_set>
#include <utility>
#include <vector>

#include "catalog/simple_catalog.h"
#include "concurrency/transaction.h"
#include "storage/page/tmp_tuple_page.h"

namespace bustub {
/**
 * ExecutorContext stores all the context necessary to run an executor.
 */
class ExecutorContext {
 public:
  /**
   * Creates an ExecutorContext for the transaction that is executing the query.
   * @param transaction the transaction executing the query
   * @param catalog the catalog that the executor should use
   * @param bpm the buffer pool manager that the executor should use
   */
  ExecutorContext(Transaction *transaction, SimpleCatalog *catalog, BufferPoolManager *bpm)
      : transaction_(transaction), catalog_{catalog}, bpm_{bpm} {}

  DISALLOW_COPY_AND_MOVE(ExecutorContext);

  ~ExecutorContext() = default;

  /** @return the running transaction */
  Transaction *GetTransaction() const { return transaction_; }

  /** @return the catalog */
  SimpleCatalog *GetCatalog() { return catalog_; }

  /** @return the buffer pool manager */
  BufferPoolManager *GetBufferPoolManager() { return bpm_; }

  /** @return the log manager - don't worry about it for now */
  LogManager *GetLogManager() { return nullptr; }

  /** @return the lock manager - don't worry about it for now */
  LockManager *GetLockManager() { return nullptr; }

 private:
  Transaction *transaction_;
  SimpleCatalog *catalog_;
  BufferPoolManager *bpm_;
};

}  // namespace bustub
