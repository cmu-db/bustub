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

TEST(TxnBonusTest, DISABLED_SerializableTest) {  // NOLINT
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

TEST(TxnBonusTest, DISABLED_ConcurrentSerializableTest) {  // NOLINT
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

TEST(TxnBonusTest, DISABLED_AbortTest) {  // NOLINT
  fmt::println(stderr, "--- AbortTest1: Simple Abort ---");
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
