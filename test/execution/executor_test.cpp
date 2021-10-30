//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// executor_test.cpp
//
// Identification: test/execution/executor_test.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>
#include <numeric>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "buffer/buffer_pool_manager_instance.h"
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
#include "execution/plans/delete_plan.h"
#include "execution/plans/distinct_plan.h"
#include "execution/plans/hash_join_plan.h"
#include "execution/plans/limit_plan.h"
#include "execution/plans/seq_scan_plan.h"
#include "execution/plans/update_plan.h"
#include "executor_test_util.h"  // NOLINT
#include "gtest/gtest.h"
#include "storage/table/tuple.h"
#include "test_util.h"  // NOLINT
#include "type/value_factory.h"

/**
 * This file contains basic tests for the functionality of all nine
 * executors required for Fall 2021 Project 3: Query Execution. In
 * particular, the tests in this file include:
 *
 * - Sequential Scan
 * - Insert (Raw)
 * - Insert (Select)
 * - Update
 * - Delete
 * - Nested Loop Join
 * - Hash Join
 * - Aggregation
 * - Limit
 * - Distinct
 *
 * Each of the tests demonstrates how to construct a query plan for
 * a particular executors. Students should be able to learn from and
 * extend these example usages to write their own tests for the
 * correct functionality of their executors.
 *
 * Each of the tests in this file uses the `ExecutorTest` unit test
 * fixture. This class is defined in the header:
 *
 * `test/execution/executor_test_util.h`
 *
 * This text fixture takes care of many of the steps required to set
 * up the system for execution engine tests. For example, it initializes
 * key DBMS components, such as the disk manager, the  buffer pool manager,
 * and the catalog, among others. Furthermore, this text fixture also
 * populates the test tables used by all unit tests. This is accomplished
 * with the help of the `TableGenerator` class via a call to `GenerateTestTables()`.
 *
 * See the definition of `TableGenerator::GenerateTestTables()` for the
 * schema of each of the tables used in the tests below. The definition of
 * this function is in `src/catalog/table_generator.cpp`.
 */

namespace bustub {

// Parameters for index construction
using KeyType = GenericKey<8>;
using ValueType = RID;
using ComparatorType = GenericComparator<8>;
using HashFunctionType = HashFunction<KeyType>;

// SELECT col_a, col_b FROM test_1 WHERE col_a < 500
TEST_F(ExecutorTest, DISABLED_SimpleSeqScanTest) {
  // Construct query plan
  TableInfo *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
  const Schema &schema = table_info->schema_;
  auto *col_a = MakeColumnValueExpression(schema, 0, "colA");
  auto *col_b = MakeColumnValueExpression(schema, 0, "colB");
  auto *const500 = MakeConstantValueExpression(ValueFactory::GetIntegerValue(500));
  auto *predicate = MakeComparisonExpression(col_a, const500, ComparisonType::LessThan);
  auto *out_schema = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
  SeqScanPlanNode plan{out_schema, predicate, table_info->oid_};

  // Execute
  std::vector<Tuple> result_set{};
  GetExecutionEngine()->Execute(&plan, &result_set, GetTxn(), GetExecutorContext());

  // Verify
  ASSERT_EQ(result_set.size(), 500);
  for (const auto &tuple : result_set) {
    ASSERT_TRUE(tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>() < 500);
    ASSERT_TRUE(tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>() < 10);
  }
}

// INSERT INTO empty_table2 VALUES (100, 10), (101, 11), (102, 12)
TEST_F(ExecutorTest, DISABLED_SimpleRawInsertTest) {
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
  const auto &schema = table_info->schema_;
  auto col_a = MakeColumnValueExpression(schema, 0, "colA");
  auto col_b = MakeColumnValueExpression(schema, 0, "colB");
  auto out_schema = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
  SeqScanPlanNode scan_plan{out_schema, nullptr, table_info->oid_};

  std::vector<Tuple> result_set{};
  GetExecutionEngine()->Execute(&scan_plan, &result_set, GetTxn(), GetExecutorContext());

  // Size
  ASSERT_EQ(result_set.size(), 3);

  // First value
  ASSERT_EQ(result_set[0].GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 100);
  ASSERT_EQ(result_set[0].GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 10);

  // Second value
  ASSERT_EQ(result_set[1].GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 101);
  ASSERT_EQ(result_set[1].GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 11);

  // Third value
  ASSERT_EQ(result_set[2].GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 102);
  ASSERT_EQ(result_set[2].GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 12);
}

// INSERT INTO empty_table2 SELECT col_a, col_b FROM test_1 WHERE col_a < 500
TEST_F(ExecutorTest, DISABLED_SimpleSelectInsertTest) {
  const Schema *out_schema1;
  std::unique_ptr<AbstractPlanNode> scan_plan1;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "colA");
    auto col_b = MakeColumnValueExpression(schema, 0, "colB");
    auto const500 = MakeConstantValueExpression(ValueFactory::GetIntegerValue(500));
    auto predicate = MakeComparisonExpression(col_a, const500, ComparisonType::LessThan);
    out_schema1 = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
    scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, predicate, table_info->oid_);
  }

  std::unique_ptr<AbstractPlanNode> insert_plan;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
    insert_plan = std::make_unique<InsertPlanNode>(scan_plan1.get(), table_info->oid_);
  }

  // Execute the insert
  GetExecutionEngine()->Execute(insert_plan.get(), nullptr, GetTxn(), GetExecutorContext());

  // Now iterate through both tables, and make sure they have the same data
  const Schema *out_schema2;
  std::unique_ptr<AbstractPlanNode> scan_plan2;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "colA");
    auto col_b = MakeColumnValueExpression(schema, 0, "colB");
    out_schema2 = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
    scan_plan2 = std::make_unique<SeqScanPlanNode>(out_schema2, nullptr, table_info->oid_);
  }

  std::vector<Tuple> result_set1{};
  std::vector<Tuple> result_set2{};
  GetExecutionEngine()->Execute(scan_plan1.get(), &result_set1, GetTxn(), GetExecutorContext());
  GetExecutionEngine()->Execute(scan_plan2.get(), &result_set2, GetTxn(), GetExecutorContext());

  ASSERT_EQ(result_set1.size(), result_set2.size());
  ASSERT_EQ(result_set1.size(), 500);

  for (std::size_t i = 0; i < result_set1.size(); ++i) {
    ASSERT_EQ(result_set1[i].GetValue(out_schema1, out_schema1->GetColIdx("colA")).GetAs<int32_t>(),
              result_set2[i].GetValue(out_schema2, out_schema2->GetColIdx("colA")).GetAs<int32_t>());
    ASSERT_EQ(result_set1[i].GetValue(out_schema1, out_schema1->GetColIdx("colB")).GetAs<int32_t>(),
              result_set2[i].GetValue(out_schema2, out_schema2->GetColIdx("colB")).GetAs<int32_t>());
  }
}

// INSERT INTO empty_table2 VALUES (100, 10), (101, 11), (102, 12)
TEST_F(ExecutorTest, DISABLED_SimpleRawInsertWithIndexTest) {
  // Create Values to insert
  std::vector<Value> val1{ValueFactory::GetIntegerValue(100), ValueFactory::GetIntegerValue(10)};
  std::vector<Value> val2{ValueFactory::GetIntegerValue(101), ValueFactory::GetIntegerValue(11)};
  std::vector<Value> val3{ValueFactory::GetIntegerValue(102), ValueFactory::GetIntegerValue(12)};
  std::vector<std::vector<Value>> raw_vals{val1, val2, val3};

  // Create insert plan node
  auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
  InsertPlanNode insert_plan{std::move(raw_vals), table_info->oid_};

  auto key_schema = ParseCreateStatement("a bigint");
  ComparatorType comparator{key_schema.get()};
  auto *index_info = GetExecutorContext()->GetCatalog()->CreateIndex<KeyType, ValueType, ComparatorType>(
      GetTxn(), "index1", "empty_table2", table_info->schema_, *key_schema, {0}, 8, HashFunctionType{});

  // Execute the insert
  GetExecutionEngine()->Execute(&insert_plan, nullptr, GetTxn(), GetExecutorContext());

  // Iterate through table make sure that values were inserted.

  // SELECT * FROM empty_table2;
  auto &schema = table_info->schema_;
  auto col_a = MakeColumnValueExpression(schema, 0, "colA");
  auto col_b = MakeColumnValueExpression(schema, 0, "colB");
  auto out_schema = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
  SeqScanPlanNode scan_plan{out_schema, nullptr, table_info->oid_};

  std::vector<Tuple> result_set{};
  GetExecutionEngine()->Execute(&scan_plan, &result_set, GetTxn(), GetExecutorContext());

  // First value
  ASSERT_EQ(result_set[0].GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 100);
  ASSERT_EQ(result_set[0].GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 10);

  // Second value
  ASSERT_EQ(result_set[1].GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 101);
  ASSERT_EQ(result_set[1].GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 11);

  // Third value
  ASSERT_EQ(result_set[2].GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 102);
  ASSERT_EQ(result_set[2].GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 12);

  // Size
  ASSERT_EQ(result_set.size(), 3);
  std::vector<RID> rids{};

  // Get RID from index, fetch tuple, and compare
  for (auto &table_tuple : result_set) {
    rids.clear();

    // Scan the index
    const auto index_key = table_tuple.KeyFromTuple(schema, index_info->key_schema_, index_info->index_->GetKeyAttrs());
    index_info->index_->ScanKey(index_key, &rids, GetTxn());

    Tuple indexed_tuple{};
    ASSERT_TRUE(table_info->table_->GetTuple(rids[0], &indexed_tuple, GetTxn()));
    ASSERT_EQ(indexed_tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(),
              table_tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>());
    ASSERT_EQ(indexed_tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(),
              table_tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>());
  }
}

// UPDATE test_3 SET colB = colB + 1;
TEST_F(ExecutorTest, DISABLED_SimpleUpdateTest) {
  // Construct a sequential scan of the table
  const Schema *out_schema{};
  std::unique_ptr<AbstractPlanNode> scan_plan{};
  {
    auto *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_3");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "colA");
    auto col_b = MakeColumnValueExpression(schema, 0, "colB");
    out_schema = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
    scan_plan = std::make_unique<SeqScanPlanNode>(out_schema, nullptr, table_info->oid_);
  }

  // Construct an update plan
  std::unique_ptr<AbstractPlanNode> update_plan{};
  std::unordered_map<uint32_t, UpdateInfo> update_attrs{};
  update_attrs.emplace(static_cast<uint32_t>(1), UpdateInfo{UpdateType::Add, 1});
  {
    auto *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_3");
    update_plan = std::make_unique<UpdatePlanNode>(scan_plan.get(), table_info->oid_, update_attrs);
  }

  std::vector<Tuple> result_set{};

  // Execute an initial sequential scan, ensure all expected tuples are present
  GetExecutionEngine()->Execute(scan_plan.get(), &result_set, GetTxn(), GetExecutorContext());

  // Verify results
  ASSERT_EQ(result_set.size(), TEST3_SIZE);

  for (auto i = 0UL; i < result_set.size(); ++i) {
    auto &tuple = result_set[i];
    ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), static_cast<int32_t>(i));
    ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), static_cast<int32_t>(i));
  }

  result_set.clear();

  // Execute update for all tuples in the table
  GetExecutionEngine()->Execute(update_plan.get(), &result_set, GetTxn(), GetExecutorContext());

  // UpdateExecutor should not modify the result set
  ASSERT_EQ(result_set.size(), 0);
  result_set.clear();

  // Execute another sequential scan; no tuples should be present in the table
  GetExecutionEngine()->Execute(scan_plan.get(), &result_set, GetTxn(), GetExecutorContext());

  // Verify results after update
  ASSERT_EQ(result_set.size(), TEST3_SIZE);

  for (auto i = 0UL; i < result_set.size(); ++i) {
    auto &tuple = result_set[i];
    ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), static_cast<int32_t>(i));
    ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), static_cast<int32_t>(i + 1));
  }
}

// DELETE FROM test_1 WHERE col_a == 50;
TEST_F(ExecutorTest, DISABLED_SimpleDeleteTest) {
  // Construct query plan
  auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
  auto &schema = table_info->schema_;
  auto col_a = MakeColumnValueExpression(schema, 0, "colA");
  auto const50 = MakeConstantValueExpression(ValueFactory::GetIntegerValue(50));
  auto predicate = MakeComparisonExpression(col_a, const50, ComparisonType::Equal);
  auto out_schema1 = MakeOutputSchema({{"colA", col_a}});
  auto scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, predicate, table_info->oid_);

  // Create the index
  auto key_schema = ParseCreateStatement("a bigint");
  ComparatorType comparator{key_schema.get()};
  auto *index_info = GetExecutorContext()->GetCatalog()->CreateIndex<KeyType, ValueType, ComparatorType>(
      GetTxn(), "index1", "test_1", GetExecutorContext()->GetCatalog()->GetTable("test_1")->schema_, *key_schema, {0},
      8, HashFunctionType{});

  std::vector<Tuple> result_set;
  GetExecutionEngine()->Execute(scan_plan1.get(), &result_set, GetTxn(), GetExecutorContext());

  // Verify
  ASSERT_EQ(result_set.size(), 1);
  for (const auto &tuple : result_set) {
    ASSERT_TRUE(tuple.GetValue(out_schema1, out_schema1->GetColIdx("colA")).GetAs<int32_t>() == 50);
  }

  // DELETE FROM test_1 WHERE col_a == 50
  const Tuple index_key = Tuple(result_set[0]);
  std::unique_ptr<AbstractPlanNode> delete_plan;
  { delete_plan = std::make_unique<DeletePlanNode>(scan_plan1.get(), table_info->oid_); }
  GetExecutionEngine()->Execute(delete_plan.get(), nullptr, GetTxn(), GetExecutorContext());

  result_set.clear();

  // SELECT col_a FROM test_1 WHERE col_a == 50
  GetExecutionEngine()->Execute(scan_plan1.get(), &result_set, GetTxn(), GetExecutorContext());
  ASSERT_TRUE(result_set.empty());

  // Ensure the key was removed from the index
  std::vector<RID> rids{};
  index_info->index_->ScanKey(index_key, &rids, GetTxn());
  ASSERT_TRUE(rids.empty());
}

// SELECT test_1.col_a, test_1.col_b, test_2.col1, test_2.col3 FROM test_1 JOIN test_2 ON test_1.col_a = test_2.col1;
TEST_F(ExecutorTest, DISABLED_SimpleNestedLoopJoinTest) {
  const Schema *out_schema1;
  std::unique_ptr<AbstractPlanNode> scan_plan1;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "colA");
    auto col_b = MakeColumnValueExpression(schema, 0, "colB");
    out_schema1 = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
    scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, nullptr, table_info->oid_);
  }

  const Schema *out_schema2;
  std::unique_ptr<AbstractPlanNode> scan_plan2;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_2");
    auto &schema = table_info->schema_;
    auto col1 = MakeColumnValueExpression(schema, 0, "col1");
    auto col3 = MakeColumnValueExpression(schema, 0, "col3");
    out_schema2 = MakeOutputSchema({{"col1", col1}, {"col3", col3}});
    scan_plan2 = std::make_unique<SeqScanPlanNode>(out_schema2, nullptr, table_info->oid_);
  }

  const Schema *out_final;
  std::unique_ptr<NestedLoopJoinPlanNode> join_plan;
  {
    // col_a and col_b have a tuple index of 0 because they are the left side of the join
    auto col_a = MakeColumnValueExpression(*out_schema1, 0, "colA");
    auto col_b = MakeColumnValueExpression(*out_schema1, 0, "colB");
    // col1 and col2 have a tuple index of 1 because they are the right side of the join
    auto col1 = MakeColumnValueExpression(*out_schema2, 1, "col1");
    auto col3 = MakeColumnValueExpression(*out_schema2, 1, "col3");
    auto predicate = MakeComparisonExpression(col_a, col1, ComparisonType::Equal);
    out_final = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}, {"col1", col1}, {"col3", col3}});
    join_plan = std::make_unique<NestedLoopJoinPlanNode>(
        out_final, std::vector<const AbstractPlanNode *>{scan_plan1.get(), scan_plan2.get()}, predicate);
  }

  std::vector<Tuple> result_set{};
  GetExecutionEngine()->Execute(join_plan.get(), &result_set, GetTxn(), GetExecutorContext());
  ASSERT_EQ(result_set.size(), 100);
}

// SELECT test_4.colA, test_4.colB, test_6.colA, test_6.colB FROM test_4 JOIN test_6 ON test_4.colA = test_6.colA;
TEST_F(ExecutorTest, DISABLED_SimpleHashJoinTest) {
  // Construct sequential scan of table test_4
  const Schema *out_schema1{};
  std::unique_ptr<AbstractPlanNode> scan_plan1{};
  {
    auto *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_4");
    auto &schema = table_info->schema_;
    auto *col_a = MakeColumnValueExpression(schema, 0, "colA");
    auto *col_b = MakeColumnValueExpression(schema, 0, "colB");
    out_schema1 = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
    scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, nullptr, table_info->oid_);
  }

  // Construct sequential scan of table test_6
  const Schema *out_schema2{};
  std::unique_ptr<AbstractPlanNode> scan_plan2{};
  {
    auto *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_6");
    auto &schema = table_info->schema_;
    auto *col_a = MakeColumnValueExpression(schema, 0, "colA");
    auto *col_b = MakeColumnValueExpression(schema, 0, "colB");
    out_schema2 = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});
    scan_plan2 = std::make_unique<SeqScanPlanNode>(out_schema2, nullptr, table_info->oid_);
  }

  // Construct the join plan
  const Schema *out_schema{};
  std::unique_ptr<HashJoinPlanNode> join_plan{};
  {
    // Columns from Table 4 have a tuple index of 0 because they are the left side of the join (outer relation)
    auto *table4_col_a = MakeColumnValueExpression(*out_schema1, 0, "colA");
    auto *table4_col_b = MakeColumnValueExpression(*out_schema1, 0, "colB");

    // Columns from Table 6 have a tuple index of 1 because they are the right side of the join (inner relation)
    auto *table6_col_a = MakeColumnValueExpression(*out_schema2, 1, "colA");
    auto *table6_col_b = MakeColumnValueExpression(*out_schema2, 1, "colB");

    out_schema = MakeOutputSchema({{"table4_colA", table4_col_a},
                                   {"table4_colB", table4_col_b},
                                   {"table6_colA", table6_col_a},
                                   {"table6_colB", table6_col_b}});

    // Join on table4.colA = table6.colA
    join_plan = std::make_unique<HashJoinPlanNode>(
        out_schema, std::vector<const AbstractPlanNode *>{scan_plan1.get(), scan_plan2.get()}, table4_col_a,
        table6_col_a);
  }

  std::vector<Tuple> result_set{};
  GetExecutionEngine()->Execute(join_plan.get(), &result_set, GetTxn(), GetExecutorContext());
  ASSERT_EQ(result_set.size(), 100);

  for (const auto &tuple : result_set) {
    const auto t4_col_a = tuple.GetValue(out_schema, out_schema->GetColIdx("table4_colA")).GetAs<int64_t>();
    const auto t4_col_b = tuple.GetValue(out_schema, out_schema->GetColIdx("table4_colB")).GetAs<int32_t>();
    const auto t6_col_a = tuple.GetValue(out_schema, out_schema->GetColIdx("table6_colA")).GetAs<int64_t>();
    const auto t6_col_b = tuple.GetValue(out_schema, out_schema->GetColIdx("table6_colB")).GetAs<int32_t>();

    // Join keys should be equiavlent
    ASSERT_EQ(t4_col_a, t6_col_a);

    // In case of Table 4 and Table 6, corresponding columns also equal
    ASSERT_LT(t4_col_b, TEST4_SIZE);
    ASSERT_LT(t6_col_b, TEST6_SIZE);
    ASSERT_EQ(t4_col_b, t6_col_b);
  }
}

// SELECT COUNT(col_a), SUM(col_a), min(col_a), max(col_a) from test_1;
TEST_F(ExecutorTest, DISABLED_SimpleAggregationTest) {
  const Schema *scan_schema;
  std::unique_ptr<AbstractPlanNode> scan_plan;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "colA");
    scan_schema = MakeOutputSchema({{"colA", col_a}});
    scan_plan = std::make_unique<SeqScanPlanNode>(scan_schema, nullptr, table_info->oid_);
  }

  const Schema *agg_schema;
  std::unique_ptr<AbstractPlanNode> agg_plan;
  {
    const AbstractExpression *col_a = MakeColumnValueExpression(*scan_schema, 0, "colA");
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
  std::vector<Tuple> result_set{};
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
  ASSERT_EQ(result_set.size(), 1);
}

// SELECT count(col_a), col_b, sum(col_c) FROM test_1 Group By col_b HAVING count(col_a) > 100
TEST_F(ExecutorTest, DISABLED_SimpleGroupByAggregation) {
  const Schema *scan_schema;
  std::unique_ptr<AbstractPlanNode> scan_plan;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto col_a = MakeColumnValueExpression(schema, 0, "colA");
    auto col_b = MakeColumnValueExpression(schema, 0, "colB");
    auto col_c = MakeColumnValueExpression(schema, 0, "colC");
    scan_schema = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}, {"colC", col_c}});
    scan_plan = std::make_unique<SeqScanPlanNode>(scan_schema, nullptr, table_info->oid_);
  }

  const Schema *agg_schema;
  std::unique_ptr<AbstractPlanNode> agg_plan;
  {
    const AbstractExpression *col_a = MakeColumnValueExpression(*scan_schema, 0, "colA");
    const AbstractExpression *col_b = MakeColumnValueExpression(*scan_schema, 0, "colB");
    const AbstractExpression *col_c = MakeColumnValueExpression(*scan_schema, 0, "colC");
    // Make group bys
    std::vector<const AbstractExpression *> group_by_cols{col_b};
    const AbstractExpression *groupby_b = MakeAggregateValueExpression(true, 0);
    // Make aggregates
    std::vector<const AbstractExpression *> aggregate_cols{col_a, col_c};
    std::vector<AggregationType> agg_types{AggregationType::CountAggregate, AggregationType::SumAggregate};
    const AbstractExpression *count_a = MakeAggregateValueExpression(false, 0);
    // Make having clause
    const AbstractExpression *having = MakeComparisonExpression(
        count_a, MakeConstantValueExpression(ValueFactory::GetIntegerValue(100)), ComparisonType::GreaterThan);

    // Create plan
    agg_schema = MakeOutputSchema({{"countA", count_a}, {"colB", groupby_b}});
    agg_plan = std::make_unique<AggregationPlanNode>(agg_schema, scan_plan.get(), having, std::move(group_by_cols),
                                                     std::move(aggregate_cols), std::move(agg_types));
  }

  std::vector<Tuple> result_set{};
  GetExecutionEngine()->Execute(agg_plan.get(), &result_set, GetTxn(), GetExecutorContext());

  std::unordered_set<int32_t> encountered{};
  for (const auto &tuple : result_set) {
    // Should have count_a > 100
    ASSERT_GT(tuple.GetValue(agg_schema, agg_schema->GetColIdx("countA")).GetAs<int32_t>(), 100);
    // Should have unique col_bs.
    auto col_b = tuple.GetValue(agg_schema, agg_schema->GetColIdx("colB")).GetAs<int32_t>();
    ASSERT_EQ(encountered.count(col_b), 0);
    encountered.insert(col_b);
    // Sanity check: col_b should also be within [0, 10).
    ASSERT_TRUE(0 <= col_b && col_b < 10);
  }
}

// SELECT colA, colB FROM test_3 LIMIT 10
TEST_F(ExecutorTest, DISABLED_SimpleLimitTest) {
  auto *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_3");
  auto &schema = table_info->schema_;

  auto *col_a = MakeColumnValueExpression(schema, 0, "colA");
  auto *col_b = MakeColumnValueExpression(schema, 0, "colB");
  auto *out_schema = MakeOutputSchema({{"colA", col_a}, {"colB", col_b}});

  // Construct sequential scan
  auto seq_scan_plan = std::make_unique<SeqScanPlanNode>(out_schema, nullptr, table_info->oid_);

  // Construct the limit plan
  auto limit_plan = std::make_unique<LimitPlanNode>(out_schema, seq_scan_plan.get(), 10);

  // Execute sequential scan with limit
  std::vector<Tuple> result_set{};
  GetExecutionEngine()->Execute(limit_plan.get(), &result_set, GetTxn(), GetExecutorContext());

  // Verify results
  ASSERT_EQ(result_set.size(), 10);
  for (auto i = 0UL; i < result_set.size(); ++i) {
    auto &tuple = result_set[i];
    ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), static_cast<int32_t>(i));
    ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), static_cast<int32_t>(i));
  }
}

// SELECT DISTINCT colC FROM test_7
TEST_F(ExecutorTest, DISABLED_SimpleDistinctTest) {
  auto *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_7");
  auto &schema = table_info->schema_;

  auto *col_c = MakeColumnValueExpression(schema, 0, "colC");
  auto *out_schema = MakeOutputSchema({{"colC", col_c}});

  // Construct sequential scan
  auto seq_scan_plan = std::make_unique<SeqScanPlanNode>(out_schema, nullptr, table_info->oid_);

  // Construct the distinct plan
  auto distinct_plan = std::make_unique<DistinctPlanNode>(out_schema, seq_scan_plan.get());

  // Execute sequential scan with DISTINCT
  std::vector<Tuple> result_set{};
  GetExecutionEngine()->Execute(distinct_plan.get(), &result_set, GetTxn(), GetExecutorContext());

  // Verify results; colC is cyclic on 0 - 9
  ASSERT_EQ(result_set.size(), 10);

  // Results are unordered
  std::vector<int32_t> results{};
  results.reserve(result_set.size());
  std::transform(result_set.cbegin(), result_set.cend(), std::back_inserter(results), [=](const Tuple &tuple) {
    return tuple.GetValue(out_schema, out_schema->GetColIdx("colC")).GetAs<int32_t>();
  });
  std::sort(results.begin(), results.end());

  // Expect keys 0 - 9
  std::vector<int32_t> expected(result_set.size());
  std::iota(expected.begin(), expected.end(), 0);

  ASSERT_TRUE(std::equal(results.cbegin(), results.cend(), expected.cbegin()));
}

}  // namespace bustub
