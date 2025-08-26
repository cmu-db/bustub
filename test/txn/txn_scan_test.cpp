//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// txn_scan_test.cpp
//
// Identification: test/txn/txn_scan_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

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

TEST(TxnScanTest, DISABLED_CollectUndoLogTest) {  // NOLINT
  auto bustub = std::make_unique<BusTubInstance>();
  auto schema = ParseCreateStatement("a integer,b double,c boolean");
  auto modify_schema = ParseCreateStatement("a integer,b double");
  auto empty_schema = ParseCreateStatement("");
  auto table_info = bustub->catalog_->CreateTable(nullptr, "maintable", *schema);

  auto txn0 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn0->GetReadTs(), 0);
  bustub->txn_manager_->Commit(txn0);
  auto rid0 = *table_info->table_->InsertTuple(TupleMeta{txn0->GetCommitTs(), false},
                                               Tuple{{Int(1), Double(1.0), BoolNull()}, schema.get()});

  auto txn1 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn1->GetReadTs(), 1);
  bustub->txn_manager_->Commit(txn1);
  auto txn_to_inspect = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn_to_inspect->GetReadTs(), 2);
  auto rid1 = *table_info->table_->InsertTuple(TupleMeta{txn1->GetCommitTs(), false},
                                               Tuple{{Int(2), Double(2.0), BoolNull()}, schema.get()});
  auto undo_link1 = txn1->AppendUndoLog(
      UndoLog{false, {true, true, false}, Tuple{{Int(1), Double(1.0)}, modify_schema.get()}, txn0->GetCommitTs(), {}});
  bustub->txn_manager_->UpdateUndoLink(rid1, undo_link1, nullptr);

  auto txn2 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn2->GetReadTs(), 2);
  bustub->txn_manager_->Commit(txn2);
  auto rid2 = *table_info->table_->InsertTuple(TupleMeta{txn2->GetCommitTs(), false},
                                               Tuple{{Int(3), Double(3.0), BoolNull()}, schema.get()});
  auto undo_link2 = txn1->AppendUndoLog(
      UndoLog{false, {true, true, false}, Tuple{{Int(1), Double(1.0)}, modify_schema.get()}, txn0->GetCommitTs(), {}});
  auto undo_link3 = txn2->AppendUndoLog(UndoLog{
      false, {true, true, false}, Tuple{{Int(2), Double(2.0)}, modify_schema.get()}, txn1->GetCommitTs(), undo_link2});
  bustub->txn_manager_->UpdateUndoLink(rid2, undo_link3, nullptr);

  auto rid3 = *table_info->table_->InsertTuple(TupleMeta{txn2->GetCommitTs(), false},
                                               Tuple{{Int(3), Double(3.0), BoolNull()}, schema.get()});

  auto txn3 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn3->GetReadTs(), 3);
  bustub->txn_manager_->Commit(txn3);
  auto rid4 = *table_info->table_->InsertTuple(TupleMeta{txn3->GetCommitTs(), false},
                                               Tuple{{Int(4), Double(4.0), BoolNull()}, schema.get()});

  auto undo_link4 = txn3->AppendUndoLog(
      UndoLog{false, {true, true, false}, Tuple{{Int(3), Double(3.0)}, modify_schema.get()}, txn2->GetCommitTs(), {}});
  bustub->txn_manager_->UpdateUndoLink(rid4, undo_link4, nullptr);

  auto rid5 = *table_info->table_->InsertTuple(TupleMeta{txn3->GetCommitTs(), true},
                                               Tuple{{Int(2), Double(2.0), BoolNull()}, schema.get()});

  auto undo_link5 = txn3->AppendUndoLog(
      UndoLog{false, {true, true, false}, Tuple{{Int(2), Double(2.0)}, modify_schema.get()}, txn1->GetCommitTs(), {}});
  bustub->txn_manager_->UpdateUndoLink(rid5, undo_link5, nullptr);
  auto rid6 = *table_info->table_->InsertTuple(TupleMeta{txn3->GetCommitTs(), false},
                                               Tuple{{Int(4), Double(4.0), BoolNull()}, schema.get()});

  auto undo_link6 = txn1->AppendUndoLog(UndoLog{
      false, {true, true, true}, Tuple{{Int(1), Double(1.0), BoolNull()}, schema.get()}, txn0->GetCommitTs(), {}});
  auto undo_link7 =
      txn3->AppendUndoLog(UndoLog{true, {false, false, false}, Tuple::Empty(), txn1->GetCommitTs(), undo_link6});
  bustub->txn_manager_->UpdateUndoLink(rid6, undo_link7, nullptr);

  auto rid7 = *table_info->table_->InsertTuple(TupleMeta{txn_to_inspect->GetTransactionTempTs(), false},
                                               Tuple{{Int(100), Double(100.0), BoolNull()}, schema.get()});
  auto rid8 = *table_info->table_->InsertTuple(TupleMeta{txn_to_inspect->GetTransactionTempTs(), false},
                                               Tuple{{Int(100), Double(100.0), BoolNull()}, schema.get()});

  auto undo_link8 = txn_to_inspect->AppendUndoLog(
      UndoLog{false, {true, true, false}, Tuple{{Int(1), Double(1.0)}, modify_schema.get()}, txn0->GetCommitTs(), {}});
  bustub->txn_manager_->UpdateUndoLink(rid8, undo_link8, nullptr);

  auto txn4 = bustub->txn_manager_->Begin();
  ASSERT_EQ(txn4->GetReadTs(), 4);

  auto rid9 = *table_info->table_->InsertTuple(TupleMeta{txn4->GetTransactionTempTs(), false},
                                               Tuple{{Int(400), Double(400.0), BoolNull()}, schema.get()});

  auto undo_link9 = txn1->AppendUndoLog(
      UndoLog{false, {true, true, false}, Tuple{{Int(1), Double(1.0)}, modify_schema.get()}, txn0->GetCommitTs(), {}});
  auto undo_link10 = txn4->AppendUndoLog(UndoLog{
      false, {true, true, false}, Tuple{{Int(4), Double(4.0)}, modify_schema.get()}, txn3->GetCommitTs(), undo_link9});
  bustub->txn_manager_->UpdateUndoLink(rid9, undo_link10, nullptr);

  TxnMgrDbg("before verify scan", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());
  auto tuple_res_0 = table_info->table_->GetTuple(rid0);
  auto tuple_res_1 = table_info->table_->GetTuple(rid1);
  auto tuple_res_2 = table_info->table_->GetTuple(rid2);
  auto tuple_res_3 = table_info->table_->GetTuple(rid3);
  auto tuple_res_4 = table_info->table_->GetTuple(rid4);
  auto tuple_res_5 = table_info->table_->GetTuple(rid5);
  auto tuple_res_6 = table_info->table_->GetTuple(rid6);
  auto tuple_res_7 = table_info->table_->GetTuple(rid7);
  auto tuple_res_8 = table_info->table_->GetTuple(rid8);
  auto tuple_res_9 = table_info->table_->GetTuple(rid9);
  auto undo_logs_0_for_txn_to_inspect =
      CollectUndoLogs(rid0, tuple_res_0.first, tuple_res_0.second, bustub->txn_manager_->GetUndoLink(rid0),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_1_for_txn_to_inspect =
      CollectUndoLogs(rid1, tuple_res_1.first, tuple_res_1.second, bustub->txn_manager_->GetUndoLink(rid1),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_2_for_txn_to_inspect =
      CollectUndoLogs(rid2, tuple_res_2.first, tuple_res_2.second, bustub->txn_manager_->GetUndoLink(rid2),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_3_for_txn_to_inspect =
      CollectUndoLogs(rid3, tuple_res_3.first, tuple_res_3.second, bustub->txn_manager_->GetUndoLink(rid3),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_4_for_txn_to_inspect =
      CollectUndoLogs(rid4, tuple_res_4.first, tuple_res_4.second, bustub->txn_manager_->GetUndoLink(rid4),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_5_for_txn_to_inspect =
      CollectUndoLogs(rid5, tuple_res_5.first, tuple_res_5.second, bustub->txn_manager_->GetUndoLink(rid5),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_6_for_txn_to_inspect =
      CollectUndoLogs(rid6, tuple_res_6.first, tuple_res_6.second, bustub->txn_manager_->GetUndoLink(rid6),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_7_for_txn_to_inspect =
      CollectUndoLogs(rid7, tuple_res_7.first, tuple_res_7.second, bustub->txn_manager_->GetUndoLink(rid7),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_8_for_txn_to_inspect =
      CollectUndoLogs(rid8, tuple_res_8.first, tuple_res_8.second, bustub->txn_manager_->GetUndoLink(rid8),
                      txn_to_inspect, bustub->txn_manager_.get());
  auto undo_logs_9_for_txn_to_inspect =
      CollectUndoLogs(rid9, tuple_res_9.first, tuple_res_9.second, bustub->txn_manager_->GetUndoLink(rid9),
                      txn_to_inspect, bustub->txn_manager_.get());

  // Reference TxnMgrDbg output at this point:
  // RID=0/0 ts=1 tuple=(1, 1.000000, <NULL>)
  // RID=0/1 ts=2 tuple=(2, 2.000000, <NULL>)
  //   txn1@0 (1, 1.000000, _) ts=1
  // RID=0/2 ts=3 tuple=(3, 3.000000, <NULL>)
  //   txn3@0 (2, 2.000000, _) ts=2
  //   txn1@1 (1, 1.000000, _) ts=1
  // RID=0/3 ts=3 tuple=(3, 3.000000, <NULL>)
  // RID=0/4 ts=4 tuple=(4, 4.000000, <NULL>)
  //   txn4@0 (3, 3.000000, _) ts=3
  // RID=0/5 ts=4 <del marker> tuple=(2, 2.000000, <NULL>)
  //   txn4@1 (2, 2.000000, _) ts=2
  // RID=0/6 ts=4 tuple=(4, 4.000000, <NULL>)
  //   txn4@2 <del> ts=2
  //   txn1@2 (1, 1.000000, <NULL>) ts=1
  // RID=0/7 ts=txn2 tuple=(100, 100.000000, <NULL>)
  // RID=0/8 ts=txn2 tuple=(100, 100.000000, <NULL>)
  //   txn2@0 (1, 1.000000, _) ts=1
  // RID=0/9 ts=txn5 tuple=(400, 400.000000, <NULL>)
  //   txn5@0 (4, 4.000000, _) ts=4
  //   txn1@3 (1, 1.000000, _) ts=1

  {
    ASSERT_TRUE(undo_logs_0_for_txn_to_inspect.has_value());
    auto tuple = ReconstructTuple(schema.get(), tuple_res_0.second, tuple_res_0.first, *undo_logs_0_for_txn_to_inspect);
    ASSERT_TRUE(tuple.has_value());
    VerifyTuple(schema.get(), *tuple, {Int(1), Double(1.0), BoolNull()});
  }

  {
    ASSERT_TRUE(undo_logs_1_for_txn_to_inspect.has_value());
    auto tuple = ReconstructTuple(schema.get(), tuple_res_1.second, tuple_res_1.first, *undo_logs_1_for_txn_to_inspect);
    ASSERT_TRUE(tuple.has_value());
    VerifyTuple(schema.get(), *tuple, {Int(2), Double(2.0), BoolNull()});
  }
  {
    ASSERT_TRUE(undo_logs_2_for_txn_to_inspect.has_value());
    auto tuple = ReconstructTuple(schema.get(), tuple_res_2.second, tuple_res_2.first, *undo_logs_2_for_txn_to_inspect);
    ASSERT_TRUE(tuple.has_value());
    VerifyTuple(schema.get(), *tuple, {Int(2), Double(2.0), BoolNull()});
  }
  { ASSERT_FALSE(undo_logs_3_for_txn_to_inspect.has_value()); }
  { ASSERT_FALSE(undo_logs_4_for_txn_to_inspect.has_value()); }
  {
    ASSERT_TRUE(undo_logs_5_for_txn_to_inspect.has_value());
    auto tuple = ReconstructTuple(schema.get(), tuple_res_5.second, tuple_res_5.first, *undo_logs_5_for_txn_to_inspect);
    ASSERT_TRUE(tuple.has_value());
    VerifyTuple(schema.get(), *tuple, {Int(2), Double(2.0), BoolNull()});
  }
  {
    ASSERT_TRUE(undo_logs_6_for_txn_to_inspect.has_value());
    auto tuple = ReconstructTuple(schema.get(), tuple_res_6.second, tuple_res_6.first, *undo_logs_6_for_txn_to_inspect);
    ASSERT_FALSE(tuple.has_value());
  }
  {
    ASSERT_TRUE(undo_logs_7_for_txn_to_inspect.has_value());
    auto tuple = ReconstructTuple(schema.get(), tuple_res_7.second, tuple_res_7.first, *undo_logs_7_for_txn_to_inspect);
    ASSERT_TRUE(tuple.has_value());
    VerifyTuple(schema.get(), *tuple, {Int(100), Double(100.0), BoolNull()});
  }
  {
    ASSERT_TRUE(undo_logs_8_for_txn_to_inspect.has_value());
    auto tuple = ReconstructTuple(schema.get(), tuple_res_8.second, tuple_res_8.first, *undo_logs_8_for_txn_to_inspect);
    ASSERT_TRUE(tuple.has_value());
    VerifyTuple(schema.get(), *tuple, {Int(100), Double(100.0), BoolNull()});
  }
  {
    ASSERT_TRUE(undo_logs_9_for_txn_to_inspect.has_value());
    auto tuple = ReconstructTuple(schema.get(), tuple_res_9.second, tuple_res_9.first, *undo_logs_9_for_txn_to_inspect);
    ASSERT_TRUE(tuple.has_value());
    VerifyTuple(schema.get(), *tuple, {Int(1), Double(1.0), BoolNull()});
  }
}

TEST(TxnScanTest, DISABLED_ScanTest) {  // NOLINT
  auto bustub = std::make_unique<BusTubInstance>();
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
  bustub->txn_manager_->UpdateUndoLink(rid1, prev_log_1, nullptr);
  auto rid2 = *table_info->table_->InsertTuple(TupleMeta{txn3->GetReadTs(), false},
                                               Tuple{{Int(3), DoubleNull(), BoolNull()}, schema.get()});
  bustub->txn_manager_->UpdateUndoLink(rid2, prev_log_2, nullptr);
  auto rid3 = *table_info->table_->InsertTuple(TupleMeta{txn4->GetReadTs(), true},
                                               Tuple{{IntNull(), DoubleNull(), BoolNull()}, schema.get()});
  bustub->txn_manager_->UpdateUndoLink(rid3, prev_log_4, nullptr);
  auto rid4 = *table_info->table_->InsertTuple(TupleMeta{txn3->GetTransactionTempTs(), true},
                                               Tuple{{IntNull(), DoubleNull(), BoolNull()}, schema.get()});
  bustub->txn_manager_->UpdateUndoLink(rid4, prev_log_5, nullptr);

  TxnMgrDbg("before verify scan", bustub->txn_manager_.get(), table_info.get(), table_info->table_.get());

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
