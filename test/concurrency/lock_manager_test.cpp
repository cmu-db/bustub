//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lock_manager_test.cpp
//
// Identification: test/concurrency/lock_manager_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <thread>  // NOLINT

#include "concurrency/lock_manager.h"
#include "concurrency/transaction_manager.h"
#include "gtest/gtest.h"

namespace bustub {

/*
 * This test is only a sanity check. Please do not rely on this test
 * to check the correctness.
 */

void BasicTest1(DeadlockMode deadlock_mode) {
  LockManager lock_mgr{TwoPLMode::REGULAR, deadlock_mode};
  TransactionManager txn_mgr{&lock_mgr};
  RID rid{0, 0};

  auto *txn0 = txn_mgr.Begin();
  auto *txn1 = txn_mgr.Begin();
  EXPECT_EQ(0, txn0->GetTransactionId());
  EXPECT_EQ(1, txn1->GetTransactionId());

  std::thread t0([&] {
    bool res = lock_mgr.LockShared(txn0, rid);
    EXPECT_EQ(true, res);
    EXPECT_EQ(TransactionState::GROWING, txn0->GetState());
    txn_mgr.Commit(txn0);
    EXPECT_EQ(TransactionState::COMMITTED, txn0->GetState());
  });

  std::thread t1([&] {
    bool res = lock_mgr.LockShared(txn1, rid);
    EXPECT_EQ(true, res);
    EXPECT_EQ(TransactionState::GROWING, txn1->GetState());
    txn_mgr.Commit(txn1);
    EXPECT_EQ(TransactionState::COMMITTED, txn1->GetState());
  });

  t0.join();
  t1.join();
  delete txn0;
  delete txn1;
}

// NOLINTNEXTLINE
TEST(LockManagerTest, DISABLED_BasicTest) {
  BasicTest1(DeadlockMode::PREVENTION);
  BasicTest1(DeadlockMode::DETECTION);
}

// NOLINTNEXTLINE
TEST(LockManagerTest, DISABLED_GraphEdgeTest) {
  LockManager lock_mgr{TwoPLMode::REGULAR, DeadlockMode::DETECTION};
  TransactionManager txn_mgr{&lock_mgr};
  RID rid{0, 0};

  lock_mgr.AddEdge(1, 2);
  auto edges = lock_mgr.GetEdgeList();

  EXPECT_EQ(1, edges.size());
  auto edge = edges[0];
  EXPECT_EQ(1, edge.first);
  EXPECT_EQ(2, edge.second);
}

// NOLINTNEXTLINE
TEST(LockManagerTest, DISABLED_BasicCycleTest) {
  LockManager lock_mgr{TwoPLMode::REGULAR, DeadlockMode::DETECTION}; /* Use Deadlock detection */
  TransactionManager txn_mgr{&lock_mgr};

  /*** Create 0->1->0 cycle ***/
  lock_mgr.AddEdge(0, 1);
  lock_mgr.AddEdge(1, 0);
  EXPECT_EQ(2, lock_mgr.GetEdgeList().size());

  txn_id_t txn;
  EXPECT_EQ(true, lock_mgr.HasCycle(&txn));
  EXPECT_EQ(1, txn);

  lock_mgr.RemoveEdge(1, 0);
  EXPECT_EQ(false, lock_mgr.HasCycle(&txn));
}

// NOLINTNEXTLINE
TEST(LockManagerTest, DISABLED_BasicDeadlockDetectionTest) {
  LockManager lock_mgr{TwoPLMode::REGULAR, DeadlockMode::DETECTION};
  cycle_detection_interval = std::chrono::milliseconds(500);
  TransactionManager txn_mgr{&lock_mgr};
  RID rid0{0, 0};
  RID rid1{1, 1};
  auto *txn0 = txn_mgr.Begin();
  auto *txn1 = txn_mgr.Begin();
  EXPECT_EQ(0, txn0->GetTransactionId());
  EXPECT_EQ(1, txn1->GetTransactionId());

  std::thread t0([&] {
    // Lock and sleep
    bool res = lock_mgr.LockExclusive(txn0, rid0);
    EXPECT_EQ(true, res);
    EXPECT_EQ(TransactionState::GROWING, txn1->GetState());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // This will block
    lock_mgr.LockExclusive(txn0, rid1);

    lock_mgr.Unlock(txn0, rid0);
    lock_mgr.Unlock(txn0, rid1);

    txn_mgr.Commit(txn0);
    EXPECT_EQ(TransactionState::COMMITTED, txn0->GetState());
  });

  std::thread t1([&] {
    // Sleep so T0 can take necessary locks
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    bool res = lock_mgr.LockExclusive(txn1, rid1);
    EXPECT_EQ(res, true);
    EXPECT_EQ(TransactionState::GROWING, txn1->GetState());

    // This will block
    res = lock_mgr.LockExclusive(txn1, rid0);
    EXPECT_EQ(res, false);

    EXPECT_EQ(TransactionState::ABORTED, txn1->GetState());
    txn_mgr.Abort(txn1);
  });

  // Sleep for enough time to break cycle
  std::this_thread::sleep_for(cycle_detection_interval * 2);

  t0.join();
  t1.join();

  delete txn0;
  delete txn1;
}
}  // namespace bustub
