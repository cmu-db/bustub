#include "execution/execution_common.h"
#include "txn_common.h"  // NOLINT

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

TEST(TxnExecutorTest, DISABLED_GarbageCollection) {  // NOLINT
  auto bustub = std::make_unique<BustubInstance>();
  Execute(*bustub, "CREATE TABLE table1(a int, b int, c int)");
  auto table_info = bustub->catalog_->GetTable("table1");
  const std::string query = "SELECT * FROM table1";
  auto txn_watermark_at_0 = BeginTxn(*bustub, "txn_watermark_at_0");
  auto txn_watermark_at_0_id = txn_watermark_at_0->GetTransactionId();
  BumpCommitTs(*bustub, 2);
  auto txn_a = BeginTxn(*bustub, "txn_a");
  auto txn_a_id = txn_a->GetTransactionId();
  WithTxn(txn_a, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO table1 VALUES (0, 0, 0), (1, 1, 1)"));
  WithTxn(txn_a, CommitTxn(*bustub, _var, _txn));
  auto txn_b = BeginTxn(*bustub, "txn_b");
  auto txn_b_id = txn_b->GetTransactionId();
  WithTxn(txn_b, ExecuteTxn(*bustub, _var, _txn, "INSERT INTO table1 VALUES (2, 2, 2), (3, 3, 3)"));
  WithTxn(txn_b, CommitTxn(*bustub, _var, _txn));
  BumpCommitTs(*bustub, 2);
  auto txn_watermark_at_1 = BeginTxn(*bustub, "txn_watermark_at_1");
  auto txn_watermark_at_1_id = txn_watermark_at_1->GetTransactionId();
  BumpCommitTs(*bustub, 2);
  auto txn2 = BeginTxn(*bustub, "txn2");
  auto txn2_id = txn2->GetTransactionId();
  WithTxn(txn2, ExecuteTxn(*bustub, _var, _txn, "UPDATE table1 SET a = a + 10"));
  WithTxn(txn2, QueryShowResult(*bustub, _var, _txn, query, IntResult{{10, 0, 0}, {11, 1, 1}, {12, 2, 2}, {13, 3, 3}}));
  WithTxn(txn2, CommitTxn(*bustub, _var, _txn));
  BumpCommitTs(*bustub, 2);
  auto txn_watermark_at_2 = BeginTxn(*bustub, "txn_watermark_at_2");
  auto txn_watermark_at_2_id = txn_watermark_at_2->GetTransactionId();
  BumpCommitTs(*bustub, 2);
  auto txn3 = BeginTxn(*bustub, "txn3");
  auto txn3_id = txn3->GetTransactionId();
  WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "UPDATE table1 SET a = a + 10 WHERE a < 12"));
  WithTxn(txn3, ExecuteTxn(*bustub, _var, _txn, "DELETE FROM table1 WHERE a = 21"));
  WithTxn(txn3, QueryShowResult(*bustub, _var, _txn, query, IntResult{{20, 0, 0}, {12, 2, 2}, {13, 3, 3}}));
  WithTxn(txn3, CommitTxn(*bustub, _var, _txn));
  BumpCommitTs(*bustub, 2);
  auto txn_watermark_at_3 = BeginTxn(*bustub, "txn_watermark_at_3");
  auto txn_watermark_at_3_id = txn_watermark_at_3->GetTransactionId();
  BumpCommitTs(*bustub, 2);
  TxnMgrDbg("after commit", bustub->txn_manager_.get(), table_info, table_info->table_.get());

  WithTxn(txn_watermark_at_0, QueryShowResult(*bustub, _var, _txn, query, IntResult{}));
  WithTxn(txn_watermark_at_1,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{0, 0, 0}, {1, 1, 1}, {2, 2, 2}, {3, 3, 3}}));
  WithTxn(txn_watermark_at_2,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{10, 0, 0}, {11, 1, 1}, {12, 2, 2}, {13, 3, 3}}));
  WithTxn(txn_watermark_at_3,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{20, 0, 0}, {12, 2, 2}, {13, 3, 3}}));

  fmt::println(stderr, "A: first GC");
  GarbageCollection(*bustub);
  TxnMgrDbg("after garbage collection", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  fmt::println(stderr, "B: second GC");
  GarbageCollection(*bustub);
  TxnMgrDbg("after garbage collection (yes, we call it twice without doing anything...)", bustub->txn_manager_.get(),
            table_info, table_info->table_.get());
  WithTxn(txn_watermark_at_0, EnsureTxnExists(*bustub, _var, txn_watermark_at_0_id));
  WithTxn(txn_watermark_at_1, EnsureTxnExists(*bustub, _var, txn_watermark_at_1_id));
  WithTxn(txn_watermark_at_2, EnsureTxnExists(*bustub, _var, txn_watermark_at_2_id));
  WithTxn(txn_watermark_at_3, EnsureTxnExists(*bustub, _var, txn_watermark_at_3_id));
  WithTxn(txn_a, EnsureTxnGCed(*bustub, _var, txn_a_id));
  WithTxn(txn_b, EnsureTxnGCed(*bustub, _var, txn_b_id));
  WithTxn(txn2, EnsureTxnExists(*bustub, _var, txn2_id));
  WithTxn(txn3, EnsureTxnExists(*bustub, _var, txn3_id));
  WithTxn(txn_watermark_at_0, QueryShowResult(*bustub, _var, _txn, query, IntResult{}));
  WithTxn(txn_watermark_at_1,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{0, 0, 0}, {1, 1, 1}, {2, 2, 2}, {3, 3, 3}}));
  WithTxn(txn_watermark_at_2,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{10, 0, 0}, {11, 1, 1}, {12, 2, 2}, {13, 3, 3}}));
  WithTxn(txn_watermark_at_3,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{20, 0, 0}, {12, 2, 2}, {13, 3, 3}}));

  fmt::println(stderr, "C: 3rd GC");
  WithTxn(txn_watermark_at_0, CommitTxn(*bustub, _var, _txn));
  GarbageCollection(*bustub);
  TxnMgrDbg("after garbage collection", bustub->txn_manager_.get(), table_info, table_info->table_.get());
  WithTxn(txn_watermark_at_0, EnsureTxnGCed(*bustub, _var, txn_watermark_at_0_id));
  WithTxn(txn_watermark_at_1, EnsureTxnExists(*bustub, _var, txn_watermark_at_1_id));
  WithTxn(txn_watermark_at_2, EnsureTxnExists(*bustub, _var, txn_watermark_at_2_id));
  WithTxn(txn_watermark_at_3, EnsureTxnExists(*bustub, _var, txn_watermark_at_3_id));
  WithTxn(txn_a, EnsureTxnGCed(*bustub, _var, txn_a_id));
  WithTxn(txn_b, EnsureTxnGCed(*bustub, _var, txn_b_id));
  WithTxn(txn2, EnsureTxnExists(*bustub, _var, txn2_id));
  WithTxn(txn3, EnsureTxnExists(*bustub, _var, txn3_id));
  WithTxn(txn_watermark_at_1,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{0, 0, 0}, {1, 1, 1}, {2, 2, 2}, {3, 3, 3}}));
  WithTxn(txn_watermark_at_2,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{10, 0, 0}, {11, 1, 1}, {12, 2, 2}, {13, 3, 3}}));
  WithTxn(txn_watermark_at_3,
          QueryShowResult(*bustub, _var, _txn, query, IntResult{{20, 0, 0}, {12, 2, 2}, {13, 3, 3}}));
}

// NOLINTEND(bugprone-unchecked-optional-access))

}  // namespace bustub
