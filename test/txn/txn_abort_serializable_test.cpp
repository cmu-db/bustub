//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// txn_abort_serializable_test.cpp
//
// Identification: test/txn/txn_abort_serializable_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "common/bustub_instance.h"
#include "concurrency/transaction.h"
#include "fmt/core.h"
#include "txn_common.h"  // NOLINT

namespace bustub {

// NOLINTBEGIN(bugprone-unchecked-optional-access)

TEST(TxnSerializableTest, DISABLED_SerializableTest) {  // NOLINT
  fmt::println(stderr, "--- SerializableTest2: Serializable ---");
  {
    auto bustub = std::make_unique<BusTubInstance>();
    EnsureIndexScan(*bustub);
    Execute(*bustub, "CREATE TABLE maintable(a int, b int primary key)");
    auto table_info = bustub->catalog_->GetTable("maintable");
    auto txn1 = BeginTxnSerializable(*bustub, "txn1");
    WithTxn(txn1,
            ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 100), (1, 101), (0, 102), (0, 103)"));
    WithTxn(txn1, CommitTxn(*bustub, _var, _txn));

    auto txn2 = BeginTxnSerializable(*bustub, "txn2");
    auto txn3 = BeginTxnSerializable(*bustub, "txn3");
    auto txn_read = BeginTxnSerializable(*bustub, "txn_read");
    WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET a = 0 WHERE a = 1"));
    WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET a = 1 WHERE a = 0"));
    TxnMgrDbg("after two updates", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    WithTxn(txn_read, ExecuteTxn(*bustub, _var, _txn, "SELECT * FROM maintable WHERE a = 0"));
    WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
    WithTxn(txn3, CommitTxn(*bustub, _var, _txn, EXPECT_FAIL));
    WithTxn(txn_read, CommitTxn(*bustub, _var, _txn));
    // test continues on Gradescope...
  }
}

TEST(TxnSerializableTest, DISABLED_ConcurrentSerializableTest) {  // NOLINT
  fmt::println(stderr, "--- SerializableTest2: Concurrent Serializable ---");
  {
    for (int i = 0; i < 10; i++) {
      auto bustub = std::make_unique<BusTubInstance>();
      EnsureIndexScan(*bustub);
      Execute(*bustub, "CREATE TABLE maintable(a int, b int primary key)");
      auto table_info = bustub->catalog_->GetTable("maintable");
      auto txn1 = BeginTxnSerializable(*bustub, "txn1");
      std::string query = "INSERT INTO maintable VALUES ";
      for (int i = 0; i < 1000; i++) {
        auto str_value = std::to_string(i + 1000);
        query += i == 0 ? "(1," + str_value + ")" : ", (1," + str_value + ")";
        auto str_value2 = std::to_string(i + 2000);
        query += ", (0, " + str_value2 + ")";
      }
      WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, query));
      WithTxn(txn1, CommitTxn(*bustub, _var, _txn));

      auto txn2 = BeginTxnSerializable(*bustub, "txn2");
      auto txn3 = BeginTxnSerializable(*bustub, "txn3");
      WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET a = 1 WHERE a = 0"));
      WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET a = 0 WHERE a = 1"));
      TxnMgrDbg("after two updates", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());

      std::vector<std::thread> commit_threads;
      const int thread_cnt = 2;
      commit_threads.reserve(thread_cnt);
      int success_cnt = 0;
      std::mutex result_mutex;

      commit_threads.emplace_back([txn2, &bustub, &result_mutex, &success_cnt]() {
        auto res = bustub->txn_manager_->Commit(txn2);
        {
          std::lock_guard<std::mutex> lck(result_mutex);
          success_cnt += static_cast<int>(res);
        }
      });
      commit_threads.emplace_back([txn3, &bustub, &result_mutex, &success_cnt]() {
        auto res = bustub->txn_manager_->Commit(txn3);
        {
          std::lock_guard<std::mutex> lck(result_mutex);
          success_cnt += static_cast<int>(res);
        }
      });

      for (auto &&thread : commit_threads) {
        thread.join();
      }
      EXPECT_EQ(success_cnt, 1);
    }
  }
}

TEST(TxnAbortTest, DISABLED_SimpleAbortTest) {  // NOLINT
  fmt::println(stderr, "--- SimpleAbortTest: Setup without primary key ---");
  auto bustub = std::make_unique<BusTubInstance>();
  Execute(*bustub, "CREATE TABLE maintable(a int, b int)");
  auto table_info = bustub->catalog_->GetTable("maintable");

  fmt::println(stderr, "A: INSERT then ABORT - ensure nothing persists");
  {
    auto txn1 = BeginTxn(*bustub, "txn1");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1,10), (1,11), (2,20)"));
    WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                  IntResult{{1, 10}, {1, 11}, {2, 20}}));
    WithTxn(txn1, AbortTxn(*bustub, _var, _txn));
  }
  {
    auto txn_check = BeginTxn(*bustub, "txn_check_after_abort_insert");
    WithTxn(txn_check, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b", IntResult{}));
    WithTxn(txn_check, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "B: Prepare committed baseline for UPDATE/DELETE abort tests");
  {
    auto txn2 = BeginTxn(*bustub, "txn2");
    WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1,10), (2,200), (3,300)"));
    WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
  }
  {
    auto txn_check = BeginTxn(*bustub, "txn_check_baseline");
    WithTxn(txn_check, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                       IntResult{{1, 10}, {2, 200}, {3, 300}}));
    WithTxn(txn_check, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "C: UPDATE then ABORT - values should revert");
  {
    auto txn3 = BeginTxn(*bustub, "txn3");
    WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET b = 0 WHERE a >= 2"));
    WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                  IntResult{{1, 10}, {2, 0}, {3, 0}}));
    WithTxn(txn3, AbortTxn(*bustub, _var, _txn));
  }
  {
    auto txn_check = BeginTxn(*bustub, "txn_check_after_abort_update");
    WithTxn(txn_check, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                       IntResult{{1, 10}, {2, 200}, {3, 300}}));
    WithTxn(txn_check, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "D: DELETE then ABORT - rows should reappear");
  {
    auto txn4 = BeginTxn(*bustub, "txn4");
    WithTxn(txn4, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable"));
    WithTxn(txn4, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b", IntResult{}));
    WithTxn(txn4, AbortTxn(*bustub, _var, _txn));
  }
  {
    auto txn_check = BeginTxn(*bustub, "txn_check_after_abort_delete");
    WithTxn(txn_check, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                       IntResult{{1, 10}, {2, 200}, {3, 300}}));
    WithTxn(txn_check, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "E: Commit with duplicates (no primary key) and verify");
  {
    auto txn5 = BeginTxn(*bustub, "txn5");
    WithTxn(txn5, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1,999), (1,1000)"));
    WithTxn(txn5, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET b = b + 1 WHERE a = 3"));
    WithTxn(txn5, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                  IntResult{{1, 10}, {1, 999}, {1, 1000}, {2, 200}, {3, 301}}));
    WithTxn(txn5, CommitTxn(*bustub, _var, _txn));
  }

  fmt::println(stderr, "F: Final verification from fresh transaction");
  {
    auto txn_final = BeginTxn(*bustub, "txn_final");
    WithTxn(txn_final, QueryShowResult(*bustub, _var, _txn, "SELECT a, b FROM maintable ORDER BY a, b",
                                       IntResult{{1, 10}, {1, 999}, {1, 1000}, {2, 200}, {3, 301}}));
    WithTxn(txn_final, CommitTxn(*bustub, _var, _txn));
  }
}

TEST(TxnAbortTest, DISABLED_AbortIndexTest) {  // NOLINT
  fmt::println(stderr, "--- AbortIndexTest1: Simple Abort ---");
  {
    auto bustub = std::make_unique<BusTubInstance>();
    EnsureIndexScan(*bustub);
    Execute(*bustub, "CREATE TABLE maintable(a int primary key, b int)");
    auto table_info = bustub->catalog_->GetTable("maintable");
    auto txn1 = BeginTxn(*bustub, "txn1");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 233), (2, 2333)"));
    WithTxn(txn1, AbortTxn(*bustub, _var, _txn));
    TxnMgrDbg("after abort", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    auto txn2 = BeginTxn(*bustub, "txn2");
    WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 2333), (2, 23333), (3, 233)"));
    WithTxn(txn2, QueryShowResult(*bustub, _var, _txn, "SELECT * FROM maintable",
                                  IntResult{
                                      {1, 2333},
                                      {2, 23333},
                                      {3, 233},
                                  }));
    TxnMgrDbg("after insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
    TxnMgrDbg("after commit", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    auto txn3 = BeginTxn(*bustub, "txn3");
    WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, "SELECT * FROM maintable",
                                  IntResult{
                                      {1, 2333},
                                      {2, 23333},
                                      {3, 233},
                                  }));
    TableHeapEntryNoMoreThan(*bustub, table_info.get(), 3);
    // test continues on Gradescope...
  }
}
// NOLINTEND(bugprone-unchecked-optional-access))

}  // namespace bustub
