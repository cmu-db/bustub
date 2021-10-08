//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// executor_test_util.h
//
// Identification: test/execution/executor_test_util.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "execution/plans/delete_plan.h"
#include "execution/plans/limit_plan.h"

#include "buffer/buffer_pool_manager_instance.h"
#include "catalog/table_generator.h"
#include "concurrency/transaction_manager.h"
#include "execution/execution_engine.h"
#include "execution/executor_context.h"
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/comparison_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/seq_scan_plan.h"
#include "gtest/gtest.h"

namespace bustub {

/**
 * The ExecutorTest class defines a test fixture for executor tests.
 * Any test that is defined as part of the `ExecutorTest` fixture
 * will have access to the helper functions defined below.
 */
class ExecutorTest : public ::testing::Test {
 public:
  /** Called before every executor test. */
  void SetUp() override {
    ::testing::Test::SetUp();

    // Initialize the database subsystems
    lock_manager_ = std::make_unique<LockManager>();
    disk_manager_ = std::make_unique<DiskManager>("executor_test.db");
    bpm_ = std::make_unique<BufferPoolManagerInstance>(32, disk_manager_.get());
    txn_mgr_ = std::make_unique<TransactionManager>(lock_manager_.get(), log_manager_.get());
    catalog_ = std::make_unique<Catalog>(bpm_.get(), lock_manager_.get(), log_manager_.get());

    // Open a transaction for the test
    txn_ = txn_mgr_->Begin();

    // Create an executor context for our executors
    exec_ctx_ =
        std::make_unique<ExecutorContext>(txn_, catalog_.get(), bpm_.get(), txn_mgr_.get(), lock_manager_.get());

    // Generate test tables
    TableGenerator gen{exec_ctx_.get()};
    gen.GenerateTestTables();

    // Construct the executor engine for the test
    execution_engine_ = std::make_unique<ExecutionEngine>(bpm_.get(), txn_mgr_.get(), catalog_.get());
  }

  /** Called after every executor test. */
  void TearDown() override {
    // Commit our transaction
    txn_mgr_->Commit(txn_);

    // Shut down the disk manager and clean up the transaction
    disk_manager_->ShutDown();
    remove("executor_test.db");
    remove("executor_test.log");
    delete txn_;
  };

  /** @return The executor context for our test instance. */
  ExecutorContext *GetExecutorContext() { return exec_ctx_.get(); }

  /** @return The execution engine for our test instance. */
  ExecutionEngine *GetExecutionEngine() { return execution_engine_.get(); }

  /** @return Get the transaction for our test instance. */
  Transaction *GetTxn() { return txn_; }

  /** @return The transaction manager for our test instance. */
  TransactionManager *GetTxnManager() { return txn_mgr_.get(); }

  /** @return The catalog for our test instance. */
  Catalog *GetCatalog() { return catalog_.get(); }

  /** @return The buffer pool manager for our test instance. */
  BufferPoolManager *GetBPM() { return bpm_.get(); }

  /** @return The lock manager for our test instance. */
  LockManager *GetLockManager() { return lock_manager_.get(); }

  /**
   * Make a column value expression.
   * @param schema The schema for the expression
   * @param tuple_idx The tuple index in a JOIN operation (0 for non-JOINs)
   * @param col_name The name of the column in the schema that is referenced
   * @return A non-owning pointer to the ColumnValueExpression
   */
  const AbstractExpression *MakeColumnValueExpression(const Schema &schema, uint32_t tuple_idx,
                                                      const std::string &col_name) {
    uint32_t col_idx = schema.GetColIdx(col_name);
    auto col_type = schema.GetColumn(col_idx).GetType();
    allocated_exprs_.emplace_back(std::make_unique<ColumnValueExpression>(tuple_idx, col_idx, col_type));
    return allocated_exprs_.back().get();
  }

  /**
   * Allocate a column value expression and return it to the caller.
   * @param schema The schema for the expression
   * @param tuple_idx The tuple index in a JOIN operation (0 for non-JOINs)
   * @param col_name The name of the column in the schema that is referenced
   * @return An owning-pointer to the ColumnValueExpression
   */
  std::unique_ptr<AbstractExpression> AllocateColumnValueExpression(const Schema &schema, uint32_t tuple_idx,
                                                                    const std::string &col_name) {
    uint32_t col_idx = schema.GetColIdx(col_name);
    auto col_type = schema.GetColumn(col_idx).GetType();
    return std::make_unique<ColumnValueExpression>(tuple_idx, col_idx, col_type);
  }

  /**
   * Make a constant value expression.
   * @param val The constant value of the expression
   * @return A non-owning pointer to the ConstantValueExpression
   */
  const AbstractExpression *MakeConstantValueExpression(const Value &val) {
    allocated_exprs_.emplace_back(std::make_unique<ConstantValueExpression>(val));
    return allocated_exprs_.back().get();
  }

  /**
   * Allocate a constant value expression and return it to the caller.
   * @param val The constant value of the expression
   * @return An owning pointer to the ConstantValueExpression
   */
  std::unique_ptr<AbstractExpression> AllocateConstantValueExpression(const Value &val) {
    return std::make_unique<ConstantValueExpression>(val);
  }

  /**
   * Make a comparison expression.
   * @param lhs The abstract expression for the left-hand side of the comparison
   * @param rhs The abstract expression for the right-hand side of the comparison
   * @param comp_type The type of the comparison operation
   * @return A non-owning pointer to the ComparisonExpression
   */
  const AbstractExpression *MakeComparisonExpression(const AbstractExpression *lhs, const AbstractExpression *rhs,
                                                     ComparisonType comp_type) {
    allocated_exprs_.emplace_back(std::make_unique<ComparisonExpression>(lhs, rhs, comp_type));
    return allocated_exprs_.back().get();
  }

  /**
   * Allocate a comparison expression and return it to the caller.
   * @param lhs The abstract expression for the left-hand side of the comparison
   * @param rhs The abstract expression for the right-hand side of the comparison
   * @param comp_type The type of the comparison operation
   * @return An owning pointer to the ComparisonExpression
   */
  std::unique_ptr<AbstractExpression> AllocateComparisonExpression(const AbstractExpression *lhs,
                                                                   const AbstractExpression *rhs,
                                                                   ComparisonType comp_type) {
    return std::make_unique<ComparisonExpression>(lhs, rhs, comp_type);
  }

  /**
   * Make an aggregate value expression.
   * @param is_group_by_term `true` if the expression is a group-by term, `false` otherwise
   * @param term_idx The index of the term in the aggregates or group-bys
   * @return A non-owning pointer to the AggregateValueExpression
   */
  const AbstractExpression *MakeAggregateValueExpression(bool is_group_by_term, uint32_t term_idx) {
    allocated_exprs_.emplace_back(
        std::make_unique<AggregateValueExpression>(is_group_by_term, term_idx, TypeId::INTEGER));
    return allocated_exprs_.back().get();
  }

  /**
   * Allocate an aggregate value expression and return it to the caller.
   * @param is_group_by_term `true` if the expression is a group-by term, `false` otherwise
   * @param term_idx The index of the term in the aggregates or group-bys
   * @return An owning pointer to the AggregateValueExpression
   */
  std::unique_ptr<AbstractExpression> AllocateAggregateValueExpression(bool is_group_by_term, uint32_t term_idx) {
    return std::make_unique<AggregateValueExpression>(is_group_by_term, term_idx, TypeId::INTEGER);
  }

  /**
   * Make an output schema.
   * @param exprs The expressions that define the columns of the output schema
   * @return A non-owning pointer to the Schema
   */
  const Schema *MakeOutputSchema(const std::vector<std::pair<std::string, const AbstractExpression *>> &exprs) {
    std::vector<Column> cols;
    cols.reserve(exprs.size());
    for (const auto &input : exprs) {
      if (input.second->GetReturnType() != TypeId::VARCHAR) {
        cols.emplace_back(input.first, input.second->GetReturnType(), input.second);
      } else {
        cols.emplace_back(input.first, input.second->GetReturnType(), MAX_VARCHAR_SIZE, input.second);
      }
    }
    allocated_output_schemas_.emplace_back(std::make_unique<Schema>(cols));
    return allocated_output_schemas_.back().get();
  }

  /**
   * Allocate an output schema and return it to the caller.
   * @param exprs The expressions that define the columns of the output schema
   * @return An owning pointer to the Schema
   */
  std::unique_ptr<Schema> AllocateOutputSchema(
      const std::vector<std::pair<std::string, const AbstractExpression *>> &exprs) {
    std::vector<Column> cols;
    cols.reserve(exprs.size());
    for (const auto &input : exprs) {
      if (input.second->GetReturnType() != TypeId::VARCHAR) {
        cols.emplace_back(input.first, input.second->GetReturnType(), input.second);
      } else {
        cols.emplace_back(input.first, input.second->GetReturnType(), MAX_VARCHAR_SIZE, input.second);
      }
    }
    return std::make_unique<Schema>(cols);
  }

 private:
  /** The transaction manager */
  std::unique_ptr<TransactionManager> txn_mgr_;
  /** The transaction context for the test */
  Transaction *txn_{nullptr};
  /** The disk manager */
  std::unique_ptr<DiskManager> disk_manager_;
  /** The log manager */
  std::unique_ptr<LogManager> log_manager_{nullptr};
  /** The lock manager */
  std::unique_ptr<LockManager> lock_manager_;
  /** The buffer pool manager */
  std::unique_ptr<BufferPoolManager> bpm_;
  /** The catalog */
  std::unique_ptr<Catalog> catalog_;
  /** The executor context for the test */
  std::unique_ptr<ExecutorContext> exec_ctx_;
  /** The execution engine */
  std::unique_ptr<ExecutionEngine> execution_engine_;
  /** The collection of allocated expressions, owned by the fixture */
  std::vector<std::unique_ptr<AbstractExpression>> allocated_exprs_;
  /** The collection of allocated schemas, owned by the fixture */
  std::vector<std::unique_ptr<Schema>> allocated_output_schemas_;

  /** The maximum size allowed for VARCHAR columns */
  static constexpr const uint32_t MAX_VARCHAR_SIZE = 128;
};

}  // namespace bustub
