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

#include "buffer/buffer_pool_manager.h"
#include "catalog/table_generator.h"
#include "concurrency/transaction_manager.h"
#include "execution/executor_context.h"
#include "execution/executor_factory.h"
#include "execution/executors/aggregation_executor.h"
#include "execution/executors/hash_join_executor.h"
#include "execution/executors/insert_executor.h"
#include "execution/expressions/aggregate_value_expression.h"
#include "execution/expressions/column_value_expression.h"
#include "execution/expressions/comparison_expression.h"
#include "execution/expressions/constant_value_expression.h"
#include "execution/plans/seq_scan_plan.h"
#include "gtest/gtest.h"
#include "type/value_factory.h"

namespace bustub {

class ExecutorTest : public ::testing::Test {
 public:
  // This function is called before every test.
  void SetUp() override {
    ::testing::Test::SetUp();
    // For each test, we create a new DiskManager, BufferPoolManager, TransactionManager, and SimpleCatalog.
    disk_manager_ = std::make_unique<DiskManager>("executor_test.db");
    bpm_ = std::make_unique<BufferPoolManager>(32, disk_manager_.get());
    txn_mgr_ = std::make_unique<TransactionManager>(lock_manager_.get(), log_manager_.get());
    catalog_ = std::make_unique<SimpleCatalog>(bpm_.get(), lock_manager_.get(), log_manager_.get());
    // Begin a new transaction, along with its executor context.
    txn_ = txn_mgr_->Begin();
    exec_ctx_ = std::make_unique<ExecutorContext>(txn_, catalog_.get(), bpm_.get());
    // Generate some test tables.
    TableGenerator gen{exec_ctx_.get()};
    gen.GenerateTestTables();
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
  std::unique_ptr<LockManager> lock_manager_ = nullptr;
  std::unique_ptr<BufferPoolManager> bpm_;
  std::unique_ptr<SimpleCatalog> catalog_;
  std::unique_ptr<ExecutorContext> exec_ctx_;
  std::vector<std::unique_ptr<AbstractExpression>> allocated_exprs_;
  std::vector<std::unique_ptr<Schema>> allocated_output_schemas_;
  static constexpr uint32_t MAX_VARCHAR_SIZE = 128;
};

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleSeqScanTest) {
  // SELECT colA, colB FROM test_1 WHERE colA < 500
  TableMetadata *table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
  Schema &schema = table_info->schema_;
  auto *colA = MakeColumnValueExpression(schema, 0, "colA");
  auto *colB = MakeColumnValueExpression(schema, 0, "colB");
  auto *const500 = MakeConstantValueExpression(ValueFactory::GetIntegerValue(500));
  auto *predicate = MakeComparisonExpression(colA, const500, ComparisonType::LessThan);
  auto *out_schema = MakeOutputSchema({{"colA", colA}, {"colB", colB}});

  SeqScanPlanNode plan{out_schema, predicate, table_info->oid_};
  auto executor = ExecutorFactory::CreateExecutor(GetExecutorContext(), &plan);
  executor->Init();
  Tuple tuple;
  uint32_t num_tuples = 0;
  std::cout << "ColA, ColB" << std::endl;
  while (executor->Next(&tuple)) {
    ASSERT_TRUE(tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>() < 500);
    ASSERT_TRUE(tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>() < 10);
    std::cout << tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>() << ", "
              << tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>() << std::endl;
    num_tuples++;
  }
  ASSERT_EQ(num_tuples, 500);
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
  auto insert_executor = ExecutorFactory::CreateExecutor(GetExecutorContext(), &insert_plan);
  insert_executor->Init();
  ASSERT_TRUE(insert_executor->Next(nullptr));

  // Iterate through table make sure that values were inserted.
  // SELECT * FROM empty_table2;
  auto &schema = table_info->schema_;
  auto colA = MakeColumnValueExpression(schema, 0, "colA");
  auto colB = MakeColumnValueExpression(schema, 0, "colB");
  auto out_schema = MakeOutputSchema({{"colA", colA}, {"colB", colB}});
  SeqScanPlanNode scan_plan{out_schema, nullptr, table_info->oid_};
  auto scan_executor = ExecutorFactory::CreateExecutor(GetExecutorContext(), &scan_plan);
  scan_executor->Init();
  Tuple tuple;
  std::cout << "ColA, ColB" << std::endl;
  // First value
  ASSERT_TRUE(scan_executor->Next(&tuple));
  ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 100);
  ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 10);
  std::cout << tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>() << ", "
            << tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>() << std::endl;
  // Second value
  ASSERT_TRUE(scan_executor->Next(&tuple));
  ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 101);
  ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 11);
  std::cout << tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>() << ", "
            << tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>() << std::endl;
  // Third value
  ASSERT_TRUE(scan_executor->Next(&tuple));
  ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>(), 102);
  ASSERT_EQ(tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>(), 12);
  std::cout << tuple.GetValue(out_schema, out_schema->GetColIdx("colA")).GetAs<int32_t>() << ", "
            << tuple.GetValue(out_schema, out_schema->GetColIdx("colB")).GetAs<int32_t>() << std::endl;
  // End
  ASSERT_FALSE(scan_executor->Next(&tuple));
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleSelectInsertTest) {
  // INSERT INTO empty_table2 SELECT colA, colB FROM test_1 WHERE colA < 500
  std::unique_ptr<AbstractPlanNode> scan_plan1;
  const Schema *out_schema1;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto colA = MakeColumnValueExpression(schema, 0, "colA");
    auto colB = MakeColumnValueExpression(schema, 0, "colB");
    auto const500 = MakeConstantValueExpression(ValueFactory::GetIntegerValue(500));
    auto predicate = MakeComparisonExpression(colA, const500, ComparisonType::LessThan);
    out_schema1 = MakeOutputSchema({{"colA", colA}, {"colB", colB}});
    scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, predicate, table_info->oid_);
  }
  std::unique_ptr<AbstractPlanNode> insert_plan;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
    insert_plan = std::make_unique<InsertPlanNode>(scan_plan1.get(), table_info->oid_);
  }
  auto insert_executor = ExecutorFactory::CreateExecutor(GetExecutorContext(), insert_plan.get());
  insert_executor->Init();
  ASSERT_TRUE(insert_executor->Next(nullptr));

  // Now iterate through both tables, and make sure they have the same data
  std::unique_ptr<AbstractPlanNode> scan_plan2;
  const Schema *out_schema2;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("empty_table2");
    auto &schema = table_info->schema_;
    auto colA = MakeColumnValueExpression(schema, 0, "colA");
    auto colB = MakeColumnValueExpression(schema, 0, "colB");
    out_schema2 = MakeOutputSchema({{"colA", colA}, {"colB", colB}});
    scan_plan2 = std::make_unique<SeqScanPlanNode>(out_schema2, nullptr, table_info->oid_);
  }

  auto scan_executor1 = ExecutorFactory::CreateExecutor(GetExecutorContext(), scan_plan1.get());
  scan_executor1->Init();
  auto scan_executor2 = ExecutorFactory::CreateExecutor(GetExecutorContext(), scan_plan2.get());
  scan_executor2->Init();
  Tuple tuple1;
  Tuple tuple2;
  uint32_t num_tuples = 0;
  while (scan_executor1->Next(&tuple1) && scan_executor2->Next(&tuple2)) {
    ASSERT_EQ(tuple1.GetValue(out_schema1, out_schema1->GetColIdx("colA")).GetAs<int32_t>(),
              tuple2.GetValue(out_schema2, out_schema2->GetColIdx("colA")).GetAs<int32_t>());
    ASSERT_EQ(tuple1.GetValue(out_schema1, out_schema1->GetColIdx("colB")).GetAs<int32_t>(),
              tuple2.GetValue(out_schema2, out_schema2->GetColIdx("colB")).GetAs<int32_t>());
    std::cout << tuple1.GetValue(out_schema1, out_schema1->GetColIdx("colA")).GetAs<int32_t>() << ", "
              << tuple1.GetValue(out_schema1, out_schema1->GetColIdx("colB")).GetAs<int32_t>() << ", "
              << tuple2.GetValue(out_schema2, out_schema2->GetColIdx("colA")).GetAs<int32_t>() << ", "
              << tuple2.GetValue(out_schema2, out_schema2->GetColIdx("colB")).GetAs<int32_t>() << std::endl;
    num_tuples++;
  }
  ASSERT_EQ(num_tuples, 500);
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleHashJoinTest) {
  // INSERT INTO empty_table2 SELECT colA, colB FROM test_1 WHERE colA < 500
  std::unique_ptr<AbstractPlanNode> scan_plan1;
  const Schema *out_schema1;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto colA = MakeColumnValueExpression(schema, 0, "colA");
    auto colB = MakeColumnValueExpression(schema, 0, "colB");
    out_schema1 = MakeOutputSchema({{"colA", colA}, {"colB", colB}});
    scan_plan1 = std::make_unique<SeqScanPlanNode>(out_schema1, nullptr, table_info->oid_);
  }
  std::unique_ptr<AbstractPlanNode> scan_plan2;
  const Schema *out_schema2;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_2");
    auto &schema = table_info->schema_;
    auto col1 = MakeColumnValueExpression(schema, 0, "col1");
    auto col2 = MakeColumnValueExpression(schema, 0, "col2");
    out_schema2 = MakeOutputSchema({{"col1", col1}, {"col2", col2}});
    scan_plan2 = std::make_unique<SeqScanPlanNode>(out_schema2, nullptr, table_info->oid_);
  }
  std::unique_ptr<HashJoinPlanNode> join_plan;
  const Schema *out_final;
  {
    // colA and colB have a tuple index of 0 because they are the left side of the join
    auto colA = MakeColumnValueExpression(*out_schema1, 0, "colA");
    auto colB = MakeColumnValueExpression(*out_schema1, 0, "colB");
    // col1 and col2 have a tuple index of 1 because they are the right side of the join
    auto col1 = MakeColumnValueExpression(*out_schema2, 1, "col1");
    auto col2 = MakeColumnValueExpression(*out_schema2, 1, "col2");
    std::vector<const AbstractExpression *> left_keys{colA};
    std::vector<const AbstractExpression *> right_keys{col1};
    auto predicate = MakeComparisonExpression(colA, col1, ComparisonType::Equal);
    out_final = MakeOutputSchema({{"colA", colA}, {"colB", colB}, {"col1", col1}, {"col2", col2}});
    join_plan = std::make_unique<HashJoinPlanNode>(
        out_final, std::vector<const AbstractPlanNode *>{scan_plan1.get(), scan_plan2.get()}, predicate,
        std::move(left_keys), std::move(right_keys));
  }

  auto executor = ExecutorFactory::CreateExecutor(GetExecutorContext(), join_plan.get());
  executor->Init();
  Tuple tuple;
  uint32_t num_tuples = 0;
  std::cout << "ColA, ColB, Col1, Col2" << std::endl;
  while (executor->Next(&tuple)) {
    std::cout << tuple.GetValue(out_final, out_schema1->GetColIdx("colA")).GetAs<int32_t>() << ", "
              << tuple.GetValue(out_final, out_schema1->GetColIdx("colB")).GetAs<int32_t>() << ", "
              << tuple.GetValue(out_final, out_schema2->GetColIdx("col1")).GetAs<int16_t>() << ", "
              << tuple.GetValue(out_final, out_schema2->GetColIdx("col2")).GetAs<int32_t>() << std::endl;

    num_tuples++;
  }
  ASSERT_EQ(num_tuples, 100);
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleAggregationTest) {
  // SELECT COUNT(colA), SUM(colA), min(colA), max(colA) from test_1;
  std::unique_ptr<AbstractPlanNode> scan_plan;
  const Schema *scan_schema;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto colA = MakeColumnValueExpression(schema, 0, "colA");
    scan_schema = MakeOutputSchema({{"colA", colA}});
    scan_plan = std::make_unique<SeqScanPlanNode>(scan_schema, nullptr, table_info->oid_);
  }

  std::unique_ptr<AbstractPlanNode> agg_plan;
  const Schema *agg_schema;
  {
    const AbstractExpression *colA = MakeColumnValueExpression(*scan_schema, 0, "colA");
    const AbstractExpression *countA = MakeAggregateValueExpression(false, 0);
    const AbstractExpression *sumA = MakeAggregateValueExpression(false, 1);
    const AbstractExpression *minA = MakeAggregateValueExpression(false, 2);
    const AbstractExpression *maxA = MakeAggregateValueExpression(false, 3);

    agg_schema = MakeOutputSchema({{"countA", countA}, {"sumA", sumA}, {"minA", minA}, {"maxA", maxA}});
    agg_plan = std::make_unique<AggregationPlanNode>(
        agg_schema, scan_plan.get(), nullptr, std::vector<const AbstractExpression *>{},
        std::vector<const AbstractExpression *>{colA, colA, colA, colA},
        std::vector<AggregationType>{AggregationType::CountAggregate, AggregationType::SumAggregate,
                                     AggregationType::MinAggregate, AggregationType::MaxAggregate});
  }

  auto executor = ExecutorFactory::CreateExecutor(GetExecutorContext(), agg_plan.get());
  executor->Init();
  Tuple tuple;
  ASSERT_TRUE(executor->Next(&tuple));
  auto countA_val = tuple.GetValue(agg_schema, agg_schema->GetColIdx("countA")).GetAs<int32_t>();
  auto sumA_val = tuple.GetValue(agg_schema, agg_schema->GetColIdx("sumA")).GetAs<int32_t>();
  auto minA_val = tuple.GetValue(agg_schema, agg_schema->GetColIdx("minA")).GetAs<int32_t>();
  auto maxA_val = tuple.GetValue(agg_schema, agg_schema->GetColIdx("maxA")).GetAs<int32_t>();
  // Should count all tuples
  ASSERT_EQ(countA_val, TEST1_SIZE);
  // Should sum from 0 to TEST1_SIZE
  ASSERT_EQ(sumA_val, TEST1_SIZE * (TEST1_SIZE - 1) / 2);
  // Minimum should be 0
  ASSERT_EQ(minA_val, 0);
  // Maximum should be TEST1_SIZE - 1
  ASSERT_EQ(maxA_val, TEST1_SIZE - 1);
  std::cout << countA_val << std::endl;
  std::cout << sumA_val << std::endl;
  std::cout << minA_val << std::endl;
  std::cout << maxA_val << std::endl;
  ASSERT_FALSE(executor->Next(&tuple));
}

// NOLINTNEXTLINE
TEST_F(ExecutorTest, DISABLED_SimpleGroupByAggregation) {
  // SELECT count(colA), colB, sum(C) FROM test_1 Group By colB HAVING count(colA) > 100
  std::unique_ptr<AbstractPlanNode> scan_plan;
  const Schema *scan_schema;
  {
    auto table_info = GetExecutorContext()->GetCatalog()->GetTable("test_1");
    auto &schema = table_info->schema_;
    auto colA = MakeColumnValueExpression(schema, 0, "colA");
    auto colB = MakeColumnValueExpression(schema, 0, "colB");
    auto colC = MakeColumnValueExpression(schema, 0, "colC");
    scan_schema = MakeOutputSchema({{"colA", colA}, {"colB", colB}, {"colC", colC}});
    scan_plan = std::make_unique<SeqScanPlanNode>(scan_schema, nullptr, table_info->oid_);
  }

  std::unique_ptr<AbstractPlanNode> agg_plan;
  const Schema *agg_schema;
  {
    const AbstractExpression *colA = MakeColumnValueExpression(*scan_schema, 0, "colA");
    const AbstractExpression *colB = MakeColumnValueExpression(*scan_schema, 0, "colB");
    const AbstractExpression *colC = MakeColumnValueExpression(*scan_schema, 0, "colC");
    // Make group bys
    std::vector<const AbstractExpression *> group_by_cols{colB};
    const AbstractExpression *groupbyB = MakeAggregateValueExpression(true, 0);
    // Make aggregates
    std::vector<const AbstractExpression *> aggregate_cols{colA, colC};
    std::vector<AggregationType> agg_types{AggregationType::CountAggregate, AggregationType::SumAggregate};
    const AbstractExpression *countA = MakeAggregateValueExpression(false, 0);
    const AbstractExpression *sumC = MakeAggregateValueExpression(false, 1);
    // Make having clause
    const AbstractExpression *having = MakeComparisonExpression(
        countA, MakeConstantValueExpression(ValueFactory::GetIntegerValue(100)), ComparisonType::GreaterThan);

    // Create plan
    agg_schema = MakeOutputSchema({{"countA", countA}, {"colB", groupbyB}, {"sumC", sumC}});
    agg_plan = std::make_unique<AggregationPlanNode>(agg_schema, scan_plan.get(), having, std::move(group_by_cols),
                                                     std::move(aggregate_cols), std::move(agg_types));
  }
  auto executor = ExecutorFactory::CreateExecutor(GetExecutorContext(), agg_plan.get());
  executor->Init();
  Tuple tuple;

  std::unordered_set<int32_t> encountered;
  std::cout << "countA, colB, sumC" << std::endl;
  while (executor->Next(&tuple)) {
    // Should have countA > 100
    ASSERT_GT(tuple.GetValue(agg_schema, agg_schema->GetColIdx("countA")).GetAs<int32_t>(), 100);
    // Should have unique colBs.
    auto colB = tuple.GetValue(agg_schema, agg_schema->GetColIdx("colB")).GetAs<int32_t>();
    ASSERT_EQ(encountered.count(colB), 0);
    encountered.insert(colB);
    // Sanity check: ColB should also be within [0, 10).
    ASSERT_TRUE(0 <= colB && colB < 10);

    std::cout << tuple.GetValue(agg_schema, agg_schema->GetColIdx("countA")).GetAs<int32_t>() << ", "
              << tuple.GetValue(agg_schema, agg_schema->GetColIdx("colB")).GetAs<int32_t>() << ", "
              << tuple.GetValue(agg_schema, agg_schema->GetColIdx("sumC")).GetAs<int32_t>() << std::endl;
  }
}

}  // namespace bustub
