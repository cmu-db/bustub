#include "execution/execution_common.h"
#include "txn_common.h"  // NOLINT

namespace bustub {

// NOLINTBEGIN(bugprone-unchecked-optional-access)

TEST(TxnScanTest, DISABLED_TupleReconstructTest) {  // NOLINT
  auto schema = ParseCreateStatement("a integer,b double,c boolean");
  {
    fmt::println(stderr, "A: only base tuple");
    auto base_tuple = Tuple{{Int(0), Double(1), BoolNull()}, schema.get()};
    auto base_meta = TupleMeta{2333, false};
    auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {});
    ASSERT_TRUE(tuple.has_value());
    VerifyTuple(schema.get(), *tuple, {Int(0), Double(1), BoolNull()});
  }
  {
    fmt::println(stderr, "B: deleted base tuple + reconstruct 1 record");
    auto base_tuple = Tuple{{IntNull(), DoubleNull(), BoolNull()}, schema.get()};
    auto base_meta = TupleMeta{2333, true};
    {
      fmt::println(stderr, "B1: verify base tuple");
      auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {});
      ASSERT_FALSE(tuple.has_value());
    }
    auto undo_log_1 = UndoLog{false, {true, true, true}, Tuple{{Int(1), Double(2), Bool(false)}, schema.get()}};
    {
      fmt::println(stderr, "B2: verify 1st record");
      auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {undo_log_1});
      ASSERT_TRUE(tuple.has_value());
      VerifyTuple(schema.get(), *tuple, {Int(1), Double(2), Bool(false)});
    }
  }
  {
    fmt::println(stderr, "C: reconstruct 4 records, one of them is empty, one of them is full change");
    auto base_tuple = Tuple{{Int(0), Double(1), BoolNull()}, schema.get()};
    auto base_meta = TupleMeta{2333, false};
    auto undo_log_1_schema = ParseCreateStatement("");
    auto undo_log_1 = UndoLog{false, {false, false, false}, Tuple{{}, undo_log_1_schema.get()}};
    auto undo_log_2_schema = ParseCreateStatement("b double");
    auto undo_log_2 = UndoLog{false, {false, true, false}, Tuple{{Double(2)}, undo_log_2_schema.get()}};
    auto undo_log_3_schema = ParseCreateStatement("a integer,c boolean");
    auto undo_log_3 = UndoLog{false, {true, false, true}, Tuple{{Int(3), Bool(false)}, undo_log_3_schema.get()}};
    auto undo_log_4 = UndoLog{false, {true, true, true}, Tuple{{Int(4), Double(4), Bool(true)}, schema.get()}};
    {
      fmt::println(stderr, "C1: verify 1st record");
      auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {undo_log_1});
      ASSERT_TRUE(tuple.has_value());
      VerifyTuple(schema.get(), *tuple, {Int(0), Double(1), BoolNull()});
    }
    {
      fmt::println(stderr, "C2: verify 2nd record");
      auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {undo_log_1, undo_log_2});
      ASSERT_TRUE(tuple.has_value());
      VerifyTuple(schema.get(), *tuple, {Int(0), Double(2), BoolNull()});
    }
    {
      fmt::println(stderr, "C3: verify 3rd record");
      auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {undo_log_1, undo_log_2, undo_log_3});
      ASSERT_TRUE(tuple.has_value());
      VerifyTuple(schema.get(), *tuple, {Int(3), Double(2), Bool(false)});
    }
    {
      fmt::println(stderr, "C4: verify 4th record");
      auto tuple =
          ReconstructTuple(schema.get(), base_tuple, base_meta, {undo_log_1, undo_log_2, undo_log_3, undo_log_4});
      ASSERT_TRUE(tuple.has_value());
      VerifyTuple(schema.get(), *tuple, {Int(4), Double(4), Bool(true)});
    }
  }
  {
    fmt::println(stderr, "D: reconstruct 4 records, two of them are delete");
    auto base_tuple = Tuple{{Int(0), Double(1), BoolNull()}, schema.get()};
    auto base_meta = TupleMeta{2333, false};
    auto delete_schema = ParseCreateStatement("");
    auto delete_log = UndoLog{true, {false, false, false}, Tuple{{}, delete_schema.get()}};
    auto undo_log_1 = UndoLog{false, {true, true, true}, Tuple{{Int(1), Double(1), Bool(false)}, schema.get()}};
    auto undo_log_2 = UndoLog{false, {true, true, true}, Tuple{{IntNull(), DoubleNull(), BoolNull()}, schema.get()}};
    {
      fmt::println(stderr, "D1: apply delete record");
      auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {delete_log});
      ASSERT_FALSE(tuple.has_value());
    }
    {
      fmt::println(stderr, "D2: verify 2nd record");
      auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {delete_log, undo_log_1});
      ASSERT_TRUE(tuple.has_value());
      VerifyTuple(schema.get(), *tuple, {Int(1), Double(1), Bool(false)});
    }
    {
      fmt::println(stderr, "D3: apply delete record");
      auto tuple = ReconstructTuple(schema.get(), base_tuple, base_meta, {delete_log, undo_log_1, delete_log});
      ASSERT_FALSE(tuple.has_value());
    }
    {
      fmt::println(stderr, "D4: verify 4th record");
      auto tuple =
          ReconstructTuple(schema.get(), base_tuple, base_meta, {delete_log, undo_log_1, delete_log, undo_log_2});
      ASSERT_TRUE(tuple.has_value());
      VerifyTuple(schema.get(), *tuple, {IntNull(), DoubleNull(), BoolNull()});
    }
  }
}

TEST(TxnScanTest, DISABLED_ScanTest) {  // NOLINT
  auto bustub = std::make_unique<BustubInstance>();
  auto schema = ParseCreateStatement("a integer,b double,c boolean");
  auto modify_schema = ParseCreateStatement("a integer");
  auto empty_schema = ParseCreateStatement("");
  auto table_info = bustub->catalog_->CreateTable(nullptr, "maintable", *schema);

  // record1: txn4 (val=1) -> ts=1 in txn4 (val=2/prev_log_1)
  // record2: ts=3 (val=3) -> ts=2 in txn_store_3 (delete/prev_log_2) -> ts=1 in txn_store_2 (val=4/prev_log_3)
  // record3: ts=4 (delete) -> ts=3 in txn_store_4 (val=5/prev_log_4)
  // record4: txn3 (delete) -> ts=2 in txn3 (val=6/prev_log_5) -> ts=1 in txn_store_2 (val=7/prev_log_6)

  UndoLink prev_log_1;
  UndoLink prev_log_2;
  UndoLink prev_log_3;
  UndoLink prev_log_4;
  UndoLink prev_log_5;
  UndoLink prev_log_6;
  auto txn0 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn0->GetReadTs(), 0);

  {
    auto txn_store_1 = bustub->txn_manager_->Begin();
    ASSERT_EQ(txn_store_1->GetReadTs(), 0);
    bustub->txn_manager_->Commit(txn_store_1);
    ASSERT_EQ(txn_store_1->GetCommitTs(), 1);
  }

  auto txn1 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn1->GetReadTs(), 1);

  {
    auto txn_store_2 = bustub->txn_manager_->Begin();
    ASSERT_EQ(txn_store_2->GetReadTs(), 1);
    prev_log_3 = txn_store_2->AppendUndoLog(
        UndoLog{false, {true, true, true}, Tuple{{Int(4), Double(4), Bool(true)}, schema.get()}, 1, {}});
    prev_log_6 =
        txn_store_2->AppendUndoLog(UndoLog{false, {true, false, false}, Tuple{{Int(7)}, modify_schema.get()}, 1, {}});
    bustub->txn_manager_->Commit(txn_store_2);
    ASSERT_EQ(txn_store_2->GetCommitTs(), 2);
  }

  auto txn2 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn2->GetReadTs(), 2);

  {
    auto txn_store_3 = bustub->txn_manager_->Begin();
    ASSERT_EQ(txn_store_3->GetReadTs(), 2);
    prev_log_2 =
        txn_store_3->AppendUndoLog(UndoLog{true, {false, false, false}, Tuple{{}, empty_schema.get()}, 2, prev_log_3});
    bustub->txn_manager_->Commit(txn_store_3);
    ASSERT_EQ(txn_store_3->GetCommitTs(), 3);
  }

  auto txn3 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn3->GetReadTs(), 3);

  prev_log_5 = txn3->AppendUndoLog(
      UndoLog{false, {true, true, true}, Tuple{{Int(6), DoubleNull(), BoolNull()}, schema.get()}, 2, prev_log_6});

  {
    auto txn_store_4 = bustub->txn_manager_->Begin();
    ASSERT_EQ(txn_store_4->GetReadTs(), 3);
    prev_log_4 = txn_store_4->AppendUndoLog(
        UndoLog{false, {true, true, true}, Tuple{{Int(5), Double(3), Bool(false)}, schema.get()}, 3, {}});
    bustub->txn_manager_->Commit(txn_store_4);
    ASSERT_EQ(txn_store_4->GetCommitTs(), 4);
  }

  auto txn4 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn4->GetReadTs(), 4);

  {
    auto txn_store_5 = bustub->txn_manager_->Begin();
    ASSERT_EQ(txn_store_5->GetReadTs(), 4);
    bustub->txn_manager_->Commit(txn_store_5);
  }

  auto txn5 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn5->GetReadTs(), 5);

  prev_log_1 = txn4->AppendUndoLog(UndoLog{false, {true, false, false}, Tuple{{Int(2)}, modify_schema.get()}, 1, {}});

  auto rid1 = *table_info->table_->InsertTuple(TupleMeta{txn4->GetTransactionTempTs(), false},
                                               Tuple{{Int(1), DoubleNull(), BoolNull()}, schema.get()});
  bustub->txn_manager_->UpdateVersionLink(rid1, VersionUndoLink{prev_log_1}, nullptr);
  auto rid2 = *table_info->table_->InsertTuple(TupleMeta{txn3->GetReadTs(), false},
                                               Tuple{{Int(3), DoubleNull(), BoolNull()}, schema.get()});
  bustub->txn_manager_->UpdateVersionLink(rid2, VersionUndoLink{prev_log_2}, nullptr);
  auto rid3 = *table_info->table_->InsertTuple(TupleMeta{txn4->GetReadTs(), true},
                                               Tuple{{IntNull(), DoubleNull(), BoolNull()}, schema.get()});
  bustub->txn_manager_->UpdateVersionLink(rid3, VersionUndoLink{prev_log_4}, nullptr);
  auto rid4 = *table_info->table_->InsertTuple(TupleMeta{txn3->GetTransactionTempTs(), true},
                                               Tuple{{IntNull(), DoubleNull(), BoolNull()}, schema.get()});
  bustub->txn_manager_->UpdateVersionLink(rid4, VersionUndoLink{prev_log_5}, nullptr);

  TxnMgrDbg("before verify scan", bustub->txn_manager_.get(), table_info, table_info->table_.get());

  auto query = "SELECT * FROM maintable";
  fmt::println(stderr, "A: Verify txn0");
  WithTxn(txn0, QueryShowResult(*bustub, _var, _txn, query, AnyResult{}));
  fmt::println(stderr, "B: Verify txn1");
  WithTxn(txn1, QueryShowResult(*bustub, _var, _txn, query,
                                AnyResult{
                                    {"2", "decimal_null", "boolean_null"},
                                    {"4", "4.000000", "true"},
                                    {"7", "decimal_null", "boolean_null"},
                                }));

  // hidden tests... this is the only hidden test case among task 1, 2, 3. We recommend you to implement `TxnMgrDbg`
  // function, draw the version chain out, and think of what should be read by each txn.

  // though we don't have null and double / bool types in task 3 and onwards, we will test them in this test case.
  // you should think about types other than integer, and think of the case where the user updates / inserts
  // a column of null.

  // query = "SELECT a FROM maintable";
  // fmt::println(stderr, "C: Verify txn2");
  // WithTxn(txn2, QueryHideResult(*bustub, _var, _txn, query, IntResult{})); // <- you will need to fill in the answer
  // fmt::println(stderr, "D: Verify txn3");
  // WithTxn(txn3, QueryHideResult(*bustub, _var, _txn, query, IntResult{})); // <- you will need to fill in the answer
  // fmt::println(stderr, "E: Verify txn4");
  // WithTxn(txn4, QueryHideResult(*bustub, _var, _txn, query, IntResult{})); // <- you will need to fill in the answer
  // fmt::println(stderr, "F: Verify txn5");
  // WithTxn(txn5, QueryHideResult(*bustub, _var, _txn, query, IntResult{})); // <- you will need to fill in the answer
}

// NOLINTEND(bugprone-unchecked-optional-access))

}  // namespace bustub
