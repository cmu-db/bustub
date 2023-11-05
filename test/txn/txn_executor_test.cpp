#include "execution/execution_common.h"
#include "txn_common.h"

namespace bustub {

// NOLINTBEGIN(bugprone-unchecked-optional-access)

TEST(TxnExecutorTest, DISABLED_InsertTest) {  // NOLINT
  auto bustub = std::make_unique<BustubInstance>();
  Execute(*bustub, "CREATE TABLE maintable(a int)");
  auto table_info = bustub->catalog_->GetTable("maintable");
  auto txn1 = BeginTxn(*bustub, "txn1");
  auto txn2 = BeginTxn(*bustub, "txn2");

  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1)"));
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (2)"));

  TxnMgrDbg("after insertion", bustub->txn_manager_.get(), table_info, table_info->table_.get());

  const std::string query = "SELECT a FROM maintable";
  fmt::println(stderr, "A: check scan txn1");
  WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1}}));
}

TEST(TxnExecutorTest, DISABLED_InsertCommitTest) {  // NOLINT
  auto bustub = std::make_unique<BustubInstance>();
  Execute(*bustub, "CREATE TABLE maintable(a int)");
  auto table_info = bustub->catalog_->GetTable("maintable");
  auto txn1 = BeginTxn(*bustub, "txn1");
  auto txn2 = BeginTxn(*bustub, "txn2");

  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1)"));
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (2)"));

  TxnMgrDbg("after insertion", bustub->txn_manager_.get(), table_info, table_info->table_.get());

  const std::string query = "SELECT a FROM maintable";
  fmt::println(stderr, "A: check scan txn1");
  WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1}}));
  fmt::println(stderr, "B: check scan txn2");
  WithTxn(txn2, QueryShowResult(*bustub, _var, _txn, query, IntResult{{2}}));
  WithTxn(txn1, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after commit txn1", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  auto txn3 = BeginTxn(*bustub, "txn3");
  fmt::println(stderr, "C: check scan txn3");
  WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1}}));
}

TEST(TxnExecutorTest, DISABLED_InsertDeleteTest) {  // NOLINT
  auto bustub = std::make_unique<BustubInstance>();
  Execute(*bustub, "CREATE TABLE maintable(a int)");
  auto table_info = bustub->catalog_->GetTable("maintable");
  auto txn1 = BeginTxn(*bustub, "txn1");
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (1)"));
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (2)"));
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO maintable VALUES (3)"));
  WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM maintable WHERE a = 3"));
  TxnMgrDbg("after 3 insert + 1 delete", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  fmt::println(stderr, "A: check scan txn1");
  const auto query = "SELECT a FROM maintable";
  WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1}, {2}}));
  WithTxn(txn1, CommitTxn(*bustub, _var, _txn));
  TxnMgrDbg("after commit", bustub->txn_manager_.get(), table_info, table_info->table_.get());
}

TEST(TxnExecutorTest, DISABLED_UpdateTest) {  // NOLINT
  {
    fmt::println(stderr, "--- UpdateTest1: no undo log ---");
    auto bustub = std::make_unique<BustubInstance>();
    Execute(*bustub, "CREATE TABLE table1(a int, b int, c int)");
    auto table_info = bustub->catalog_->GetTable("table1");
    auto txn1 = BeginTxn(*bustub, "txn1");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO table1 VALUES (1, 1, 1)"));
    TxnMgrDbg("after insert", bustub->txn_manager_.get(), table_info, table_info->table_.get());
    const std::string query = "SELECT * FROM table1";
    fmt::println(stderr, "A: 1st update");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "UPDATE table1 SET b = 2"));
    TxnMgrDbg("after update", bustub->txn_manager_.get(), table_info, table_info->table_.get());
    WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 2, 1}}));
    WithTxn(txn1, CheckUndoLogNum(*bustub, _var, _txn, 0));
  }
  {
    fmt::println(stderr, "--- UpdateTest2: update applied on insert ---");
    auto bustub = std::make_unique<BustubInstance>();
    Execute(*bustub, "CREATE TABLE table2(a int, b int, c int)");
    auto table_info = bustub->catalog_->GetTable("table2");
    auto txn0 = BeginTxn(*bustub, "txn0");
    WithTxn(txn0, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO table2 VALUES (1, 1, 1)"));
    WithTxn(txn0, CommitTxn(*bustub, _var, _txn));
    TxnMgrDbg("after insert and commit", bustub->txn_manager_.get(), table_info, table_info->table_.get());
    auto txn1 = BeginTxn(*bustub, "txn1");
    auto txn_ref = BeginTxn(*bustub, "txn_ref");
    const std::string query = "SELECT * FROM table2";
    fmt::println(stderr, "A: 1st update");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "UPDATE table2 SET b = 2"));
    TxnMgrDbg("after update", bustub->txn_manager_.get(), table_info, table_info->table_.get());
    WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 2, 1}}));
    WithTxn(txn_ref, QueryShowResult(*bustub, _var, _txn, query, IntResult{{1, 1, 1}}));
    WithTxn(txn1, CheckUndoLogColumn(*bustub, _var, _txn, 1));
  }
}

TEST(TxnExecutorTest, DISABLED_UpdateConflict) {  // NOLINT
  {
    fmt::println(stderr, "--- UpdateConflict1: simple case, insert and two txn update it ---");
    auto bustub = std::make_unique<BustubInstance>();
    Execute(*bustub, "CREATE TABLE table1(a int, b int, c int)");
    auto table_info = bustub->catalog_->GetTable("table1");
    auto txn0 = BeginTxn(*bustub, "txn0");
    WithTxn(txn0, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO table1 VALUES (0, 0, 0)"));
    WithTxn(txn0, CommitTxn(*bustub, _var, _txn));
    TxnMgrDbg("after initialize", bustub->txn_manager_.get(), table_info, table_info->table_.get());
    auto txn1 = BeginTxn(*bustub, "txn1");
    auto txn2 = BeginTxn(*bustub, "txn2");
    WithTxn(txn1, ExecuteTxn(*bustub, _var, _txn, "UPDATE table1 SET a = 1"));
    TxnMgrDbg("after 1st update", bustub->txn_manager_.get(), table_info, table_info->table_.get());
    WithTxn(txn2, ExecuteTxnTainted(*bustub, _var, _txn, "UPDATE table1 SET b = 2"));
    TxnMgrDbg("after txn tainted", bustub->txn_manager_.get(), table_info, table_info->table_.get());
    WithTxn(txn1, CommitTxn(*bustub, _var, _txn));
    TxnMgrDbg("after commit", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  }
}

// NOLINTEND(bugprone-unchecked-optional-access))

}  // namespace bustub
