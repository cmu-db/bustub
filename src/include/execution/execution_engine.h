//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// execution_engine.h
//
// Identification: src/include/execution/execution_engine.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "catalog/catalog.h"
#include "concurrency/transaction_manager.h"
#include "execution/executor_context.h"
#include "execution/executor_factory.h"
#include "execution/plans/abstract_plan.h"
#include "storage/table/tuple.h"
namespace bustub {

/**
 * The ExecutionEngine class executes query plans.
 */
class ExecutionEngine {
 public:
  /**
   * Construct a new ExecutionEngine instance.
   * @param bpm The buffer pool manager used by the execution engine
   * @param txn_mgr The transaction manager used by the execution engine
   * @param catalog The catalog used by the execution engine
   */
  ExecutionEngine(BufferPoolManager *bpm, TransactionManager *txn_mgr, Catalog *catalog)
      : bpm_{bpm}, txn_mgr_{txn_mgr}, catalog_{catalog} {}

  DISALLOW_COPY_AND_MOVE(ExecutionEngine);

  /**
   * Execute a query plan.
   * @param plan The query plan to execute
   * @param result_set The set of tuples produced by executing the plan
   * @param txn The transaction context in which the query executes
   * @param exec_ctx The executor context in which the query executes
   * @return `true` if execution of the query plan succeeds, `false` otherwise
   */
  bool Execute(const AbstractPlanNode *plan, std::vector<Tuple> *result_set, Transaction *txn,
               ExecutorContext *exec_ctx) {
    // Construct and executor for the plan
    auto executor = ExecutorFactory::CreateExecutor(exec_ctx, plan);

    // Prepare the root executor
    executor->Init();

    // Execute the query plan
    try {
      Tuple tuple;
      RID rid;
      while (executor->Next(&tuple, &rid)) {
        if (result_set != nullptr) {
          result_set->push_back(tuple);
        }
      }
    } catch (Exception &e) {
      // TODO(student): handle exceptions
    }

    return true;
  }

 private:
  /** The buffer pool manager used during query execution */
  [[maybe_unused]] BufferPoolManager *bpm_;
  /** The transaction manager used during query execution */
  [[maybe_unused]] TransactionManager *txn_mgr_;
  /** The catalog used during query execution */
  [[maybe_unused]] Catalog *catalog_;
};

}  // namespace bustub
