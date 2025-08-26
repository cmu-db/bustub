//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// txn_index_test.cpp
//
// Identification: test/txn/txn_index_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <chrono>  // NOLINT
#include <exception>
#include <memory>
#include <mutex>   // NOLINT
#include <thread>  // NOLINT
#include "common/bustub_instance.h"
#include "common/macros.h"
#include "concurrency/transaction.h"
#include "execution/execution_common.h"
#include "fmt/core.h"
#include "txn_common.h"  // NOLINT

namespace bustub {

// NOLINTBEGIN(bugprone-unchecked-optional-access)

TEST(TxnIndexTest, DISABLED_IndexInsertTest) {  // NOLINT
  auto bustub = std::make_unique<BusTubInstance>();
  const std::string query = "SELECT * FROM maintable";

  Execute(*bustub, "CREATE TABLE maintable(a int primary key, b int)");
  auto table_info = bustub->catalog_->GetTable("maintable");
  auto txn1 = BeginTxn(*bustub, "txn1");
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 0)"));
  TxnMgrDbg("after txn1 insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {1, 0},
                                }));
  WithTxn(txn1, ExecuteTxnTainted(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 1)"));
  TxnMgrDbg("after txn1 taint", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  TableHeapEntryNoMoreThan(*bustub, table_info.get(), 1);

  auto txn2 = BeginTxn(*bustub, "txn2");
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (2, 2)"));
  TxnMgrDbg("after txn2 insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  WithTxn(txn2, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                }));
  WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn2 commit", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  TableHeapEntryNoMoreThan(*bustub, table_info.get(), 2);

  auto txn3 = BeginTxn(*bustub, "txn3");
  WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (3, 3)"));
  TxnMgrDbg("after txn3 insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                    {3, 3},
                                }));
  WithTxn(txn3, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn3 commit", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  TableHeapEntryNoMoreThan(*bustub, table_info.get(), 3);

  auto txn4 = BeginTxn(*bustub, "txn4");
  WithTxn(txn4, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                    {3, 3},
                                }));
  WithTxn(txn4, ExecuteTxnTainted(*bustub, _var, _txn, "INSERT INTO maintable VALUES (3, 4)"));
  TxnMgrDbg("after txn4 taint", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  TableHeapEntryNoMoreThan(*bustub, table_info.get(), 3);

  auto txn5 = BeginTxn(*bustub, "txn5");
  WithTxn(txn5, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (4, 4)"));
  TxnMgrDbg("after txn5 insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  WithTxn(txn5, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                    {3, 3},
                                    {4, 4},
                                }));
  TableHeapEntryNoMoreThan(*bustub, table_info.get(), 4);

  auto txn6 = BeginTxn(*bustub, "txn6");
  WithTxn(txn6, QueryShowResult(*bustub, _var, _txn, query,
                                IntResult{
                                    {2, 2},
                                    {3, 3},
                                }));
}

TEST(TxnIndexTest, DISABLED_InsertDeleteTest) {  // NOLINT
  const std::string query = "SELECT * FROM maintable";

  auto bustub = std::make_unique<BusTubInstance>();
  EnsureIndexScan(*bustub);
  Execute(*bustub, "CREATE TABLE maintable(col1 int primary key, col2 int)");
  auto table_info = bustub->catalog_->GetTable("maintable");

  auto txn1 = BeginTxn(*bustub, "txn1");
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 0), (2, 0), (3, 0), (4, 0)"));
  WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 0}, {2, 0}, {3, 0}, {4, 0}}));
  WithTxn(txn1, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4},
                           IntResult{{1, 0}, {2, 0}, {3, 0}, {4, 0}}));
  WithTxn(txn1, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn1 insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  auto txn1_reverify = BeginTxn(*bustub, "txn1_reverify");

  auto txn2 = BeginTxn(*bustub, "txn2");
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable"));
  WithTxn(txn2, QueryShowResult(*bustub, _var, _txn, query, IntResult{}));
  WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn2 delete", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());

  TableHeapEntryNoMoreThan(*bustub, table_info.get(), 4);

  // hidden tests in-between

  WithTxn(txn1_reverify, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 0}, {2, 0}, {3, 0}, {4, 0}}));
  WithTxn(txn1_reverify, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4},
                                    IntResult{{1, 0}, {2, 0}, {3, 0}, {4, 0}}));
}

TEST(TxnIndexTest, DISABLED_UpdateTest) {  // NOLINT
  const std::string query = "SELECT * FROM maintable";

  const auto prepare =
      [](std::unique_ptr<BusTubInstance> &bustub) -> std::tuple<Transaction *, Transaction *, Transaction *> {
    auto table_info = bustub->catalog_->GetTable("maintable");
    auto txn1 = BeginTxn(*bustub, "txn1");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 0), (2, 0)"));
    WithTxn(txn1, CommitTxn(*bustub, _var, _txn));
    auto txn1_reverify = BeginTxn(*bustub, "txn1_reverify");
    auto txn2 = BeginTxn(*bustub, "txn2");
    WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable WHERE col1 = 2"));
    WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (3, 0), (5, 0)"));
    WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable WHERE col1 = 3"));
    WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
    auto txn2_reverify = BeginTxn(*bustub, "txn2_reverify");
    // at this point, we have (1, 0) inserted, (2, 0) deleted, and (3, 0) self inserted and deleted.
    auto txn3 = BeginTxn(*bustub, "txn3");
    WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (4, 0), (6, 0)"));
    WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable WHERE col1 = 5"));
    WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable WHERE col1 = 6"));
    TxnMgrDbg("after preparation", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    // at this point, we have (4, 0) inserted, (5, 0) deleted, and (6, 0) self inserted and deleted.
    return {txn1_reverify, txn2_reverify, txn3};
  };

  const auto reverify = [](std::unique_ptr<BusTubInstance> &bustub, Transaction *txn1_reverify,
                           Transaction *txn2_reverify, const std::string &query) {
    WithTxn(txn1_reverify, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 0}, {2, 0}}));
    WithTxn(txn1_reverify, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4, 5, 6},
                                      IntResult{{1, 0}, {2, 0}, {}, {}, {}, {}}));
    WithTxn(txn2_reverify, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 0}, {5, 0}}));
    WithTxn(txn2_reverify, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4, 5, 6},
                                      IntResult{{1, 0}, {}, {}, {}, {5, 0}, {}}));
  };

  {
    fmt::println(stderr, "---- UpdateTest1: insert, update, and commit ----");
    auto bustub = std::make_unique<BusTubInstance>();
    EnsureIndexScan(*bustub);
    Execute(*bustub, "CREATE TABLE maintable(col1 int primary key, col2 int)");
    auto table_info = bustub->catalog_->GetTable("maintable");
    auto [txn1_reverify, txn2_reverify, txn3] = prepare(bustub);
    WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 0}, {4, 0}}));
    WithTxn(txn3, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4, 5, 6},
                             IntResult{{1, 0}, {}, {}, {4, 0}, {}, {}}));

    WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (2, 1), (5, 1), (3, 1), (6, 1)"));
    TxnMgrDbg("after txn3 insert operations", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET col2 = col2 + 10"));
    TxnMgrDbg("after txn3 update operations", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
    WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, query,
                                  IntResult{
                                      {1, 10},
                                      {2, 11},
                                      {3, 11},
                                      {4, 10},
                                      {5, 11},
                                      {6, 11},
                                  }));
    WithTxn(txn3, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4, 5, 6},
                             IntResult{
                                 {1, 10},
                                 {2, 11},
                                 {3, 11},
                                 {4, 10},
                                 {5, 11},
                                 {6, 11},
                             }));
    WithTxn(txn3, CommitTxn(*bustub, _var, _txn));
    TxnMgrDbg("after commit", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());

    auto txn4 = BeginTxn(*bustub, "txn4");
    WithTxn(txn4, QueryShowResult(*bustub, _var, _txn, query,
                                  IntResult{
                                      {1, 10},
                                      {2, 11},
                                      {3, 11},
                                      {4, 10},
                                      {5, 11},
                                      {6, 11},
                                  }));
    WithTxn(txn4, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4, 5, 6},
                             IntResult{
                                 {1, 10},
                                 {2, 11},
                                 {3, 11},
                                 {4, 10},
                                 {5, 11},
                                 {6, 11},
                             }));
    reverify(bustub, txn1_reverify, txn2_reverify, query);
    TableHeapEntryNoMoreThan(*bustub, table_info.get(), 6);
  }
  // hidden tests...
}

TEST(GradingTxnIndexTest, DISABLED_IndexUpdateConflictTest) {  // NOLINT
  const std::string query = "SELECT * FROM maintable";

  auto bustub = std::make_unique<BusTubInstance>();
  EnsureIndexScan(*bustub);
  Execute(*bustub, "CREATE TABLE maintable(col1 int primary key, col2 int)");
  auto table_info = bustub->catalog_->GetTable("maintable");
  auto txn1 = BeginTxn(*bustub, "txn1");
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 0), (2, 0), (3, 0)"));
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable WHERE col1 = 2"));
  WithTxn(txn1, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn1 insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  auto txn2 = BeginTxn(*bustub, "txn2");
  auto txn3 = BeginTxn(*bustub, "txn3");
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (4, 0)"));
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable WHERE col1 = 1"));
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable WHERE col1 = 3"));
  WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn2 modification", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  WithTxn(txn3, ExecuteTxnTainted(*bustub, _var, _txn, "UPDATE maintable SET col2 = 2 WHERE col1 = 1"));
  TxnMgrDbg("after txn3 tainted", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  // hidden tests...
}

TEST(TxnIndexTest, DISABLED_UpdatePrimaryKeyTest) {  // NOLINT
  const std::string query = "SELECT * FROM maintable";

  auto bustub = std::make_unique<BusTubInstance>();
  EnsureIndexScan(*bustub);
  Execute(*bustub, "CREATE TABLE maintable(col1 int primary key, col2 int)");
  auto table_info = bustub->catalog_->GetTable("maintable");
  auto txn1 = BeginTxn(*bustub, "txn1");
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1, 0), (2, 0), (3, 0), (4, 0)"));
  WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 0}, {2, 0}, {3, 0}, {4, 0}}));
  WithTxn(txn1, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4},
                           IntResult{{1, 0}, {2, 0}, {3, 0}, {4, 0}}));
  WithTxn(txn1, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn1 insert", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  auto txn2 = BeginTxn(*bustub, "txn2");
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET col1 = col1 + 1"));
  WithTxn(txn2, QueryShowResult(*bustub, _var, _txn, query, IntResult{{2, 0}, {3, 0}, {4, 0}, {5, 0}}));
  WithTxn(txn2, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{1, 2, 3, 4, 5},
                           IntResult{{}, {2, 0}, {3, 0}, {4, 0}, {5, 0}}));
  WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after txn2 update", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  auto txn3 = BeginTxn(*bustub, "txn3");
  WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "UPDATE maintable SET col1 = col1 - 2"));
  WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, query, IntResult{{0, 0}, {1, 0}, {2, 0}, {3, 0}}));
  WithTxn(txn3, QueryIndex(*bustub, _var, _txn, query, "col1", std::vector<int>{0, 1, 2, 3, 4, 5},
                           IntResult{{0, 0}, {1, 0}, {2, 0}, {3, 0}, {}, {}}));
  WithTxn(txn3, CommitTxn(*bustub, _var, _txn));
  // hidden tests...
}

// NOLINTEND(bugprone-unchecked-optional-access))

}  // namespace bustub
