//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// executor_test.cpp
//
// Identification: test/execution/executor_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "execution/plans/delete_plan.h"
#include "execution/plans/limit_plan.h"

#include "buffer/buffer_pool_manager_instance.h"
#include "catalog/table_generator.h"
#include "concurrency/transaction_manager.h"
#include "execution/execution_engine.h"
#include "execution/executor_context.h"
#include "execution/executors/aggregation_executor.h"
#include "execution/executors/insert_executor.h"
#include "execution/executors/nested_loop_join_executor.h"
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/comparison_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/seq_scan_plan.h"
#include "gtest/gtest.h"
#include "storage/b_plus_tree_test_util.h"  // NOLINT
#include "storage/table/tuple.h"
#include "type/value_factory.h"

namespace bustub {

class ExecutorTest : public ::testing::Test {
 public:
  // This function is called before every test.
  void SetUp() override {
    ::testing::Test::SetUp();
    // For each test, we create a new DiskManager, BufferPoolManager, TransactionManager, and Catalog.
    disk_manager_ = std::make_unique<DiskManager>("executor_test.db");
    bpm_ = std::make_unique<BufferPoolManagerInstance>(32, disk_manager_.get());
    page_id_t page_id;
    bpm_->NewPage(&page_id);
    lock_manager_ = std::make_unique<LockManager>();
    txn_mgr_ = std::make_unique<TransactionManager>(lock_manager_.get(), log_manager_.get());
    catalog_ = std::make_unique<Catalog>(bpm_.get(), lock_manager_.get(), log_manager_.get());
    // Begin a new transaction, along with its executor context.
    txn_ = txn_mgr_->Begin();
    exec_ctx_ =
        std::make_unique<ExecutorContext>(txn_, catalog_.get(), bpm_.get(), txn_mgr_.get(), lock_manager_.get());
    // Generate some test tables.
    TableGenerator gen{exec_ctx_.get()};
    gen.GenerateTestTables();

    execution_engine_ = std::make_unique<ExecutionEngine>(bpm_.get(), txn_mgr_.get(), catalog_.get());
  }

  // This function is called after every test.
  void TearDown() override {
    // Commit our transaction.
    txn_mgr_->Commit(txn_);
    // Shut down the disk manager and clean up the transaction.
    disk_manager_->ShutDown();
    remove("executor_test.db");
    delete txn_;
  };

  /** @return the executor context in our test class */
  ExecutorContext *GetExecutorContext() { return exec_ctx_.get(); }
  ExecutionEngine *GetExecutionEngine() { return execution_engine_.get(); }
  Transaction *GetTxn() { return txn_; }
  TransactionManager *GetTxnManager() { return txn_mgr_.get(); }
  Catalog *GetCatalog() { return catalog_.get(); }
  BufferPoolManager *GetBPM() { return bpm_.get(); }
  LockManager *GetLockManager() { return lock_manager_.get(); }

  // The below helper functions are useful for testing.

  const AbstractExpression *MakeColumnValueExpression(const Schema &schema, uint32_t tuple_idx,
                                                      const std::string &col_name) {
    uint32_t col_idx = schema.GetColIdx(col_name);
    auto col_type = schema.GetColumn(col_idx).GetType();
    allocated_exprs_.emplace_back(std::make_unique<ColumnValueExpression>(tuple_idx, col_idx, col_type));
    return allocated_exprs_.back().get();
  }

  const AbstractExpression *MakeConstantValueExpression(const Value &val) {
    allocated_exprs_.emplace_back(std::make_unique<ConstantValueExpression>(val));
    return allocated_exprs_.back().get();
  }

  const AbstractExpression *MakeComparisonExpression(const AbstractExpression *lhs, const AbstractExpression *rhs,
                                                     ComparisonType comp_type) {
    allocated_exprs_.emplace_back(std::make_unique<ComparisonExpression>(lhs, rhs, comp_type));
    return allocated_exprs_.back().get();
  }

  const AbstractExpression *MakeAggregateValueExpression(bool is_group_by_term, uint32_t term_idx) {
    allocated_exprs_.emplace_back(
        std::make_unique<AggregateValueExpression>(is_group_by_term, term_idx, TypeId::INTEGER));
    return allocated_exprs_.back().get();
  }

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

 private:
  std::unique_ptr<TransactionManager> txn_mgr_;
  Transaction *txn_{nullptr};
  std::unique_ptr<DiskManager> disk_manager_;
  std::unique_ptr<LogManager> log_manager_ = nullptr;
  std::unique_ptr<LockManager> lock_manager_;
  std::unique_ptr<BufferPoolManager> bpm_;
  std::unique_ptr<Catalog> catalog_;
  std::unique_ptr<ExecutorContext> exec_ctx_;
  std::unique_ptr<ExecutionEngine> execution_engine_;
  std::vector<std::unique_ptr<AbstractExpression>> allocated_exprs_;
  std::vector<std::unique_ptr<Schema>> allocated_output_schemas_;
  static constexpr uint32_t MAX_VARCHAR_SIZE = 128;
};

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleSeqScanTest) {
  // SELECT col_a, col_b FROM test_1 WHERE col_a < 500

  // Construct query plan
  TableMetadata *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
  Schema &schema = table_info->schema_;
  auto *col_a = MakeColumnValueExpression(schema, 0, "col_a");
  auto *col_b = MakeColumnValueExpression(schema, 0, "col_b");
  auto *const500 = MakeConstantValueExpression(ValueFactory::GetIntegerValue(500));
  auto *predicate = MakeComparisonExpression(col_a, const500, ComparisonType::LessThan);
  auto *out_schema = MakeOutputSchema({{"col_a", col_a}, {"col_b", col_b}});
  SeqScanPlanNode plan{out_schema, predicate, table_info->oid_};

  // Execute
  std::vector<Tuple> result_set;
  GetExecutionEngine()->Execute(&plan, &result_set, GetTxn(), GetExecutorContext());

  // Verify
  std::cout << "col_a, col_b" << std::endl;
  for (const auto &tuple : result_set) {
    ASSERT_TRUE(tuple.GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() < 500);
    ASSERT_TRUE(tuple.GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() < 10);
    std::cout << tuple.GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() << ", "
              << tuple.GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  }
  ASSERT_EQ(result_set.size(), 500);
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleRawInsertTest) {
  // INSERT INTO empty_table2 VALUES (100, 10), (101, 11), (102, 12)
  // Create Values to insert
  std::vector<Value> val1{ValueFactory::GetIntegerValue(100), ValueFactory::GetIntegerValue(10)};
  std::vector<Value> val2{ValueFactory::GetIntegerValue(101), ValueFactory::GetIntegerValue(11)};
  std::vector<Value> val3{ValueFactory::GetIntegerValue(102), ValueFactory::GetIntegerValue(12)};
  std::vector<std::vector<Value>> raw_vals{val1, val2, val3};
  // Create insert plan node
  auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
  InsertPlanNode insert_plan{std::move(raw_vals), table_info->oid_};

  GetExecutionEngine()->Execute(&insert_plan, nullptr, GetTxn(), GetExecutorContext());

  // Iterate through table make sure that values were inserted.
  // SELECT * FROM empty_table2;
  auto &schema = table_info->schema_;
  auto col_a = MakeColumnValueExpression(schema, 0, "col_a");
  auto col_b = MakeColumnValueExpression(schema, 0, "col_b");
  auto out_schema = MakeOutputSchema({{"col_a", col_a}, {"col_b", col_b}});
  SeqScanPlanNode scan_plan{out_schema, nullptr, table_info->oid_};

  std::vector<Tuple> result_set;
  GetExecutionEngine()->Execute(&scan_plan, &result_set, GetTxn(), GetExecutorContext());

  std::cout << "col_a, col_b" << std::endl;
  // First value
  ASSERT_EQ(result_set[0].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>(), 100);
  ASSERT_EQ(result_set[0].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>(), 10);
  std::cout << result_set[0].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() << ", "
            << result_set[0].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  // Second value
  ASSERT_EQ(result_set[1].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>(), 101);
  ASSERT_EQ(result_set[1].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>(), 11);
  std::cout << result_set[1].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() << ", "
            << result_set[1].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  // Third value
  ASSERT_EQ(result_set[2].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>(), 102);
  ASSERT_EQ(result_set[2].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>(), 12);
  std::cout << result_set[2].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() << ", "
            << result_set[2].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  // Size
  ASSERT_EQ(result_set.size(), 3);
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleSelectInsertTest) {
  // INSERT INTO empty_table2 SELECT col_a, col_b FROM test_1 WHERE col_a < 500
  std::unique_ptr<AbstractPlanNode> scan_plan1;
  const Schema *out_schema1;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "col_a");
    auto col_b = MakeColumnValueExpression(schema, 0, "col_b");
    auto const500 = MakeConstantValueExpression(ValueFactory::GetIntegerValue(500));
    auto predicate = MakeComparisonExpression(col_a, const500, ComparisonType::LessThan);
    out_schema1 = MakeOutputSchema({{"col_a", col_a}, {"col_b", col_b}});
    scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, predicate, table_info->oid_);
  }
  std::unique_ptr<AbstractPlanNode> insert_plan;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
    insert_plan = std::make_unique<InsertPlanNode>(scan_plan1.get(), table_info->oid_);
  }
  GetExecutionEngine()->Execute(insert_plan.get(), nullptr, GetTxn(), GetExecutorContext());

  // Now iterate through both tables, and make sure they have the same data
  std::unique_ptr<AbstractPlanNode> scan_plan2;
  const Schema *out_schema2;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "col_a");
    auto col_b = MakeColumnValueExpression(schema, 0, "col_b");
    out_schema2 = MakeOutputSchema({{"col_a", col_a}, {"col_b", col_b}});
    scan_plan2 = std::make_unique<SeqScanPlanNode>(out_schema2, nullptr, table_info->oid_);
  }
  std::vector<Tuple> result_set1;
  std::vector<Tuple> result_set2;
  GetExecutionEngine()->Execute(scan_plan1.get(), &result_set1, GetTxn(), GetExecutorContext());
  GetExecutionEngine()->Execute(scan_plan2.get(), &result_set2, GetTxn(), GetExecutorContext());

  ASSERT_EQ(result_set1.size(), result_set2.size());
  for (size_t i = 0; i < result_set1.size(); ++i) {
    ASSERT_EQ(result_set1[i].GetValue(out_schema1, out_schema1->GetColIdx("col_a")).GetAs<int32_t>(),
              result_set2[i].GetValue(out_schema2, out_schema2->GetColIdx("col_a")).GetAs<int32_t>());
    ASSERT_EQ(result_set1[i].GetValue(out_schema1, out_schema1->GetColIdx("col_b")).GetAs<int32_t>(),
              result_set2[i].GetValue(out_schema2, out_schema2->GetColIdx("col_b")).GetAs<int32_t>());
    std::cout << result_set1[i].GetValue(out_schema1, out_schema1->GetColIdx("col_a")).GetAs<int32_t>() << ", "
              << result_set1[i].GetValue(out_schema1, out_schema1->GetColIdx("col_b")).GetAs<int32_t>() << ", "
              << result_set2[i].GetValue(out_schema2, out_schema2->GetColIdx("col_a")).GetAs<int32_t>() << ", "
              << result_set2[i].GetValue(out_schema2, out_schema2->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  }
  ASSERT_EQ(result_set1.size(), 500);
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleRawInsertWithIndexTest) {
  // INSERT INTO empty_table2 VALUES (100, 10), (101, 11), (102, 12)
  // Create Values to insert
  std::vector<Value> val1{ValueFactory::GetIntegerValue(100), ValueFactory::GetIntegerValue(10)};
  std::vector<Value> val2{ValueFactory::GetIntegerValue(101), ValueFactory::GetIntegerValue(11)};
  std::vector<Value> val3{ValueFactory::GetIntegerValue(102), ValueFactory::GetIntegerValue(12)};
  std::vector<std::vector<Value>> raw_vals{val1, val2, val3};
  // Create insert plan node
  auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
  InsertPlanNode insert_plan{std::move(raw_vals), table_info->oid_};

  Schema *key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema);
  auto index_info = GetExecutorContext()->GetCatalog()->CreateIndex<GenericKey<8>, RID, GenericComparator<8>>(
      GetTxn(), "index1", "empty_table2", table_info->schema_, *key_schema, {0}, 8);

  GetExecutionEngine()->Execute(&insert_plan, nullptr, GetTxn(), GetExecutorContext());

  // Iterate through table make sure that values were inserted.
  // SELECT * FROM empty_table2;
  auto &schema = table_info->schema_;
  auto col_a = MakeColumnValueExpression(schema, 0, "col_a");
  auto col_b = MakeColumnValueExpression(schema, 0, "col_b");
  auto out_schema = MakeOutputSchema({{"col_a", col_a}, {"col_b", col_b}});
  SeqScanPlanNode scan_plan{out_schema, nullptr, table_info->oid_};

  std::vector<Tuple> result_set;
  GetExecutionEngine()->Execute(&scan_plan, &result_set, GetTxn(), GetExecutorContext());

  std::cout << "col_a, col_b" << std::endl;
  // First value
  ASSERT_EQ(result_set[0].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>(), 100);
  ASSERT_EQ(result_set[0].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>(), 10);
  std::cout << result_set[0].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() << ", "
            << result_set[0].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  // Second value
  ASSERT_EQ(result_set[1].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>(), 101);
  ASSERT_EQ(result_set[1].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>(), 11);
  std::cout << result_set[1].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() << ", "
            << result_set[1].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  // Third value
  ASSERT_EQ(result_set[2].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>(), 102);
  ASSERT_EQ(result_set[2].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>(), 12);
  std::cout << result_set[2].GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() << ", "
            << result_set[2].GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  // Size
  ASSERT_EQ(result_set.size(), 3);
  std::vector<RID> rids;

  // Get RID from index, fetch tuple and then compare
  for (auto &table_tuple : result_set) {
    rids.clear();
    auto index_key = table_tuple.KeyFromTuple(schema, index_info->key_schema_, index_info->index_->GetKeyAttrs());
    index_info->index_->ScanKey(index_key, &rids, GetTxn());
    Tuple indexed_tuple;
    auto fetch_tuple = table_info->table_->GetTuple(rids[0], &indexed_tuple, GetTxn());

    ASSERT_TRUE(fetch_tuple);
    ASSERT_EQ(indexed_tuple.GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>(),
              table_tuple.GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>());
    ASSERT_EQ(indexed_tuple.GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>(),
              table_tuple.GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>());

    std::cout << indexed_tuple.GetValue(out_schema, out_schema->GetColIdx("col_a")).GetAs<int32_t>() << ", "
              << indexed_tuple.GetValue(out_schema, out_schema->GetColIdx("col_b")).GetAs<int32_t>() << std::endl;
  }
  delete key_schema;
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleDeleteTest) {
  // SELECT col_a FROM test_1 WHERE col_a == 50
  // DELETE FROM test_1 WHERE col_a == 50
  // SELECT col_a FROM test_1 WHERE col_a == 50

  // Construct query plan
  auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
  auto &schema = table_info->schema_;
  auto col_a = MakeColumnValueExpression(schema, 0, "col_a");
  auto const50 = MakeConstantValueExpression(ValueFactory::GetIntegerValue(50));
  auto predicate = MakeComparisonExpression(col_a, const50, ComparisonType::Equal);
  auto out_schema1 = MakeOutputSchema({{"col_a", col_a}});
  auto scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, predicate, table_info->oid_);
  // index
  Schema *key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema);
  auto index_info = GetExecutorContext()->GetCatalog()->CreateIndex<GenericKey<8>, RID, GenericComparator<8>>(
      GetTxn(), "index1", "test_1", GetExecutorContext()->GetCatalog()->GetTable("test_1")->schema_, *key_schema, {0},
      8);

  // Execute
  std::vector<Tuple> result_set;
  GetExecutionEngine()->Execute(scan_plan1.get(), &result_set, GetTxn(), GetExecutorContext());

  // Verify
  std::cout << "col_a" << std::endl;
  for (const auto &tuple : result_set) {
    std::cout << tuple.GetValue(out_schema1, out_schema1->GetColIdx("col_a")).GetAs<int32_t>() << std::endl;
    ASSERT_TRUE(tuple.GetValue(out_schema1, out_schema1->GetColIdx("col_a")).GetAs<int32_t>() == 50);
  }
  ASSERT_EQ(result_set.size(), 1);
  Tuple index_key = Tuple(result_set[0]);

  std::unique_ptr<AbstractPlanNode> delete_plan;
  { delete_plan = std::make_unique<DeletePlanNode>(scan_plan1.get(), table_info->oid_); }
  GetExecutionEngine()->Execute(delete_plan.get(), nullptr, GetTxn(), GetExecutorContext());

  result_set.clear();
  GetExecutionEngine()->Execute(scan_plan1.get(), &result_set, GetTxn(), GetExecutorContext());
  ASSERT_TRUE(result_set.empty());

  std::vector<RID> rids;

  index_info->index_->ScanKey(index_key, &rids, GetTxn());
  ASSERT_TRUE(rids.empty());

  delete key_schema;
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleNestedLoopJoinTest) {
  // SELECT test_1.col_a, test_1.col_b, test_2.col1, test_2.col3 FROM test_1 JOIN test_2 ON test_1.col_a = test_2.col1
  std::unique_ptr<AbstractPlanNode> scan_plan1;
  const Schema *out_schema1;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "col_a");
    auto col_b = MakeColumnValueExpression(schema, 0, "col_b");
    out_schema1 = MakeOutputSchema({{"col_a", col_a}, {"col_b", col_b}});
    scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, nullptr, table_info->oid_);
  }
  std::unique_ptr<AbstractPlanNode> scan_plan2;
  const Schema *out_schema2;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_2");
    auto &schema = table_info->schema_;
    auto col1 = MakeColumnValueExpression(schema, 0, "col1");
    auto col3 = MakeColumnValueExpression(schema, 0, "col3");
    out_schema2 = MakeOutputSchema({{"col1", col1}, {"col3", col3}});
    scan_plan2 = std::make_unique<SeqScanPlanNode>(out_schema2, nullptr, table_info->oid_);
  }
  std::unique_ptr<NestedLoopJoinPlanNode> join_plan;
  const Schema *out_final;
  {
    // col_a and col_b have a tuple index of 0 because they are the left side of the join
    auto col_a = MakeColumnValueExpression(*out_schema1, 0, "col_a");
    auto col_b = MakeColumnValueExpression(*out_schema1, 0, "col_b");
    // col1 and col2 have a tuple index of 1 because they are the right side of the join
    auto col1 = MakeColumnValueExpression(*out_schema2, 1, "col1");
    auto col3 = MakeColumnValueExpression(*out_schema2, 1, "col3");
    auto predicate = MakeComparisonExpression(col_a, col1, ComparisonType::Equal);
    out_final = MakeOutputSchema({{"col_a", col_a}, {"col_b", col_b}, {"col1", col1}, {"col3", col3}});
    join_plan = std::make_unique<NestedLoopJoinPlanNode>(
        out_final, std::vector<const AbstractPlanNode *>{scan_plan1.get(), scan_plan2.get()}, predicate);
  }

  std::vector<Tuple> result_set;
  GetExecutionEngine()->Execute(join_plan.get(), &result_set, GetTxn(), GetExecutorContext());
  ASSERT_EQ(result_set.size(), 100);
  std::cout << "col_a, col_b, Col1, Col3" << std::endl;
  for (const auto &tuple : result_set) {
    std::cout << tuple.GetValue(out_final, out_final->GetColIdx("col_a")).GetAs<int32_t>() << ", "
              << tuple.GetValue(out_final, out_final->GetColIdx("col_b")).GetAs<int32_t>() << ", "
              << tuple.GetValue(out_final, out_final->GetColIdx("col1")).GetAs<int16_t>() << ", "
              << tuple.GetValue(out_final, out_final->GetColIdx("col3")).GetAs<int32_t>() << ", " << std::endl;
  }
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleAggregationTest) {
  // SELECT COUNT(col_a), SUM(col_a), min(col_a), max(col_a) from test_1;
  std::unique_ptr<AbstractPlanNode> scan_plan;
  const Schema *scan_schema;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "col_a");
    scan_schema = MakeOutputSchema({{"col_a", col_a}});
    scan_plan = std::make_unique<SeqScanPlanNode>(scan_schema, nullptr, table_info->oid_);
  }

  std::unique_ptr<AbstractPlanNode> agg_plan;
  const Schema *agg_schema;
  {
    const AbstractExpression *col_a = MakeColumnValueExpression(*scan_schema, 0, "col_a");
    const AbstractExpression *count_a = MakeAggregateValueExpression(false, 0);
    const AbstractExpression *sum_a = MakeAggregateValueExpression(false, 1);
    const AbstractExpression *min_a = MakeAggregateValueExpression(false, 2);
    const AbstractExpression *max_a = MakeAggregateValueExpression(false, 3);

    agg_schema = MakeOutputSchema({{"count_a", count_a}, {"sum_a", sum_a}, {"min_a", min_a}, {"max_a", max_a}});
    agg_plan = std::make_unique<AggregationPlanNode>(
        agg_schema, scan_plan.get(), nullptr, std::vector<const AbstractExpression *>{},
        std::vector<const AbstractExpression *>{col_a, col_a, col_a, col_a},
        std::vector<AggregationType>{AggregationType::CountAggregate, AggregationType::SumAggregate,
                                     AggregationType::MinAggregate, AggregationType::MaxAggregate});
  }
  std::vector<Tuple> result_set;
  GetExecutionEngine()->Execute(agg_plan.get(), &result_set, GetTxn(), GetExecutorContext());

  auto count_a_val = result_set[0].GetValue(agg_schema, agg_schema->GetColIdx("count_a")).GetAs<int32_t>();
  auto sum_a_val = result_set[0].GetValue(agg_schema, agg_schema->GetColIdx("sum_a")).GetAs<int32_t>();
  auto min_a_val = result_set[0].GetValue(agg_schema, agg_schema->GetColIdx("min_a")).GetAs<int32_t>();
  auto max_a_val = result_set[0].GetValue(agg_schema, agg_schema->GetColIdx("max_a")).GetAs<int32_t>();
  // Should count all tuples
  ASSERT_EQ(count_a_val, TEST1_SIZE);
  // Should sum from 0 to TEST1_SIZE
  ASSERT_EQ(sum_a_val, TEST1_SIZE * (TEST1_SIZE - 1) / 2);
  // Minimum should be 0
  ASSERT_EQ(min_a_val, 0);
  // Maximum should be TEST1_SIZE - 1
  ASSERT_EQ(max_a_val, TEST1_SIZE - 1);
  std::cout << count_a_val << std::endl;
  std::cout << sum_a_val << std::endl;
  std::cout << min_a_val << std::endl;
  std::cout << max_a_val << std::endl;
  ASSERT_EQ(result_set.size(), 1);
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleGroupByAggregation) {
  // SELECT count(col_a), col_b, sum(col_c) FROM test_1 Group By col_b HAVING count(col_a) > 100
  std::unique_ptr<AbstractPlanNode> scan_plan;
  const Schema *scan_schema;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "col_a");
    auto col_b = MakeColumnValueExpression(schema, 0, "col_b");
    auto col_c = MakeColumnValueExpression(schema, 0, "col_c");
    scan_schema = MakeOutputSchema({{"col_a", col_a}, {"col_b", col_b}, {"col_c", col_c}});
    scan_plan = std::make_unique<SeqScanPlanNode>(scan_schema, nullptr, table_info->oid_);
  }

  std::unique_ptr<AbstractPlanNode> agg_plan;
  const Schema *agg_schema;
  {
    const AbstractExpression *col_a = MakeColumnValueExpression(*scan_schema, 0, "col_a");
    const AbstractExpression *col_b = MakeColumnValueExpression(*scan_schema, 0, "col_b");
    const AbstractExpression *col_c = MakeColumnValueExpression(*scan_schema, 0, "col_c");
    // Make group bys
    std::vector<const AbstractExpression *> group_by_cols{col_b};
    const AbstractExpression *groupby_b = MakeAggregateValueExpression(true, 0);
    // Make aggregates
    std::vector<const AbstractExpression *> aggregate_cols{col_a, col_c};
    std::vector<AggregationType> agg_types{AggregationType::CountAggregate, AggregationType::SumAggregate};
    const AbstractExpression *count_a = MakeAggregateValueExpression(false, 0);
    const AbstractExpression *sum_c = MakeAggregateValueExpression(false, 1);
    // Make having clause
    const AbstractExpression *having = MakeComparisonExpression(
        count_a, MakeConstantValueExpression(ValueFactory::GetIntegerValue(100)), ComparisonType::GreaterThan);

    // Create plan
    agg_schema = MakeOutputSchema({{"count_a", count_a}, {"col_b", groupby_b}, {"sum_c", sum_c}});
    agg_plan = std::make_unique<AggregationPlanNode>(agg_schema, scan_plan.get(), having, std::move(group_by_cols),
                                                     std::move(aggregate_cols), std::move(agg_types));
  }

  std::vector<Tuple> result_set;
  GetExecutionEngine()->Execute(agg_plan.get(), &result_set, GetTxn(), GetExecutorContext());

  std::unordered_set<int32_t> encountered;
  std::cout << "count_a, col_b, sum_c" << std::endl;
  for (const auto &tuple : result_set) {
    // Should have count_a > 100
    ASSERT_GT(tuple.GetValue(agg_schema, agg_schema->GetColIdx("count_a")).GetAs<int32_t>(), 100);
    // Should have unique col_bs.
    auto col_b = tuple.GetValue(agg_schema, agg_schema->GetColIdx("col_b")).GetAs<int32_t>();
    ASSERT_EQ(encountered.count(col_b), 0);
    encountered.insert(col_b);
    // Sanity check: col_b should also be within [0, 10).
    ASSERT_TRUE(0 <= col_b && col_b < 10);

    std::cout << tuple.GetValue(agg_schema, agg_schema->GetColIdx("count_a")).GetAs<int32_t>() << ", "
              << tuple.GetValue(agg_schema, agg_schema->GetColIdx("col_b")).GetAs<int32_t>() << ", "
              << tuple.GetValue(agg_schema, agg_schema->GetColIdx("sum_c")).GetAs<int32_t>() << std::endl;
  }
}

}  // namespace bustub
