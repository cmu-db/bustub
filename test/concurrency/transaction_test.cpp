//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// transaction_test.cpp
//
// Identification: test/concurrency/transaction_test.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "concurrency/transaction.h"

#include <atomic>
#include <cstdio>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "buffer/buffer_pool_manager_instance.h"
#include "catalog/table_generator.h"
#include "common/bustub_instance.h"
#include "concurrency/transaction_manager.h"
#include "execution/execution_engine.h"
#include "execution/executor_context.h"
#include "execution/executors/insert_executor.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/comparison_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/limit_plan.h"
#include "execution/plans/nested_index_join_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "gtest/gtest.h"
#include "test_util.h"  // NOLINT
#include "type/value_factory.h"

namespace bustub {

class TransactionTest : public ::testing::Test {
 public:
  // This function is called before every test.
  void SetUp() override {
    ::testing::Test::SetUp();
    bustub_ = std::make_unique<BustubInstance>("executor_test.db");
  }

  // This function is called after every test.
  void TearDown() override { remove("executor_test.db"); };

  std::unique_ptr<BustubInstance> bustub_;
};

// --- Helper functions ---
void CheckGrowing(Transaction *txn) { EXPECT_EQ(txn->GetState(), TransactionState::GROWING); }

void CheckShrinking(Transaction *txn) { EXPECT_EQ(txn->GetState(), TransactionState::SHRINKING); }

void CheckAborted(Transaction *txn) { EXPECT_EQ(txn->GetState(), TransactionState::ABORTED); }

void CheckCommitted(Transaction *txn) { EXPECT_EQ(txn->GetState(), TransactionState::COMMITTED); }

void CheckTxnRowLockSize(Transaction *txn, size_t shared_size, size_t exclusive_size) {
  EXPECT_EQ(txn->GetSharedLockSet()->size(), shared_size);
  EXPECT_EQ(txn->GetExclusiveLockSet()->size(), exclusive_size);
}

// NOLINTNEXTLINE
TEST_F(TransactionTest, DISABLED_SimpleInsertRollbackTest) {
  // txn1: INSERT INTO empty_table2 VALUES (200, 20), (201, 21), (202, 22)
  // txn1: abort
  // txn2: SELECT * FROM empty_table2;

  auto noop_writer = NoopWriter();
  bustub_->ExecuteSql("CREATE TABLE empty_table2 (x int, y int);", noop_writer);

  auto *txn1 = bustub_->txn_manager_->Begin();
  bustub_->ExecuteSqlTxn("INSERT INTO empty_table2 VALUES(200, 20), (201, 21), (202, 22)", noop_writer, txn1);
  bustub_->txn_manager_->Abort(txn1);
  delete txn1;

  auto *txn2 = bustub_->txn_manager_->Begin();
  std::stringstream ss;
  auto writer2 = SimpleStreamWriter(ss, true);
  bustub_->ExecuteSqlTxn("SELECT * FROM empty_table2", writer2, txn2);
  EXPECT_EQ(ss.str(), "");
  bustub_->txn_manager_->Commit(txn2);
  delete txn2;
}

// NOLINTNEXTLINE
TEST_F(TransactionTest, DISABLED_DirtyReadsTest) {
  bustub_->GenerateTestTable();

  // txn1: INSERT INTO empty_table2 VALUES (200, 20), (201, 21), (202, 22)
  // txn2: SELECT * FROM empty_table2;
  // txn1: abort

  auto noop_writer = NoopWriter();

  bustub_->ExecuteSql("CREATE TABLE empty_table2 (colA int, colB int)", noop_writer);

  auto *txn1 = bustub_->txn_manager_->Begin(nullptr, IsolationLevel::READ_UNCOMMITTED);
  bustub_->ExecuteSqlTxn("INSERT INTO empty_table2 VALUES (200, 20), (201, 21), (202, 22)", noop_writer, txn1);

  auto *txn2 = bustub_->txn_manager_->Begin(nullptr, IsolationLevel::READ_UNCOMMITTED);
  std::stringstream ss;
  auto writer2 = SimpleStreamWriter(ss, true);
  bustub_->ExecuteSqlTxn("SELECT * FROM empty_table2", writer2, txn2);

  EXPECT_EQ(ss.str(), "200\t20\t\n201\t21\t\n202\t22\t\n");

  bustub_->txn_manager_->Commit(txn2);
  delete txn2;

  bustub_->txn_manager_->Abort(txn1);
  delete txn1;
}

}  // namespace bustub
