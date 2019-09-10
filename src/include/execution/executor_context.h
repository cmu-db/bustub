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

#include <utility>
#include <vector>

#include "concurrency/transaction.h"

namespace bustub {
namespace executor {

class ExecutorContext {
 public:
  /**
   * Creates an ExecutorContext for the transaction that is executing the query.
   * @param transaction the transaction executing the query
   */
  explicit ExecutorContext(Transaction *transaction) : transaction_(transaction) {}

  /**
   * Creates an ExecutorContext for the transaction that is executing the query with the given parameters.
   * @param transaction transaction executing the query
   * @param params parameters of the query
   */
  ExecutorContext(Transaction *transaction, std::vector<bustub::Value> params)
      : transaction_(transaction), params_(std::move(params)) {}

  DISALLOW_COPY_AND_MOVE(ExecutorContext);

  /** @return the running transaction */
  Transaction *GetTransaction() const { return transaction_; }

  /** @return the vector of parameters */
  const std::vector<bustub::Value> &GetParams() const { return params_; }

  ~ExecutorContext() = default;

 private:
  Transaction *transaction_;
  std::vector<bustub::Value> params_;
};

}  // namespace executor
}  // namespace bustub
