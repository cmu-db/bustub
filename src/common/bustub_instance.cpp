#include "common/bustub_instance.h"
#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/statement/create_statement.h"
#include "binder/statement/explain_statement.h"
#include "binder/statement/index_statement.h"
#include "binder/statement/select_statement.h"
#include "buffer/buffer_pool_manager_instance.h"
#include "catalog/schema.h"
#include "catalog/table_generator.h"
#include "common/enums/statement_type.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "concurrency/lock_manager.h"
#include "execution/execution_engine.h"
#include "execution/executor_context.h"
#include "execution/expressions/abstract_expression.h"
#include "execution/plans/abstract_plan.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "optimizer/optimizer.h"
#include "planner/planner.h"
#include "recovery/checkpoint_manager.h"
#include "recovery/log_manager.h"
#include "storage/disk/disk_manager.h"
#include "storage/disk/disk_manager_memory.h"

namespace bustub {

auto BustubInstance::MakeExecutorContext(Transaction *txn) -> std::unique_ptr<ExecutorContext> {
  return std::make_unique<ExecutorContext>(txn, catalog_, buffer_pool_manager_, transaction_manager_, lock_manager_);
}

BustubInstance::BustubInstance(const std::string &db_file_name) {
  // TODO(chi): revisit this when designing the recovery project.

  enable_logging = false;

  // Storage related.
  disk_manager_ = new DiskManager(db_file_name);

  // Log related.
  log_manager_ = new LogManager(disk_manager_);

  // We need more frames for GenerateTestTable to work. Therefore, we use 128 instead of the default
  // buffer pool size specified in `config.h`.
  try {
    buffer_pool_manager_ = new BufferPoolManagerInstance(128, disk_manager_, LRUK_REPLACER_K, log_manager_);
  } catch (NotImplementedException &e) {
    std::cerr << "BufferPoolManager is not implemented, only mock tables are supported." << std::endl;
    buffer_pool_manager_ = nullptr;
  }

  // Transaction (txn) related.
  lock_manager_ = new LockManager();
  transaction_manager_ = new TransactionManager(lock_manager_, log_manager_);

  // Checkpoint related.
  checkpoint_manager_ = new CheckpointManager(transaction_manager_, log_manager_, buffer_pool_manager_);

  // Catalog.
  catalog_ = new Catalog(buffer_pool_manager_, lock_manager_, log_manager_);

  // Execution engine.
  execution_engine_ = new ExecutionEngine(buffer_pool_manager_, transaction_manager_, catalog_);
}

auto BustubInstance::ExecuteSql(const std::string &sql) -> std::vector<std::string> {
  if (!sql.empty() && sql[0] == '\\') {
    auto internal_sql = StringUtil::Strip(sql, '\n');
    // Internal meta-commands, like in `psql`.
    if (internal_sql == "\\dt") {
      std::string result;
      auto table_names = catalog_->GetTableNames();
      for (const auto &name : table_names) {
        result += name;
        result += "\n";
      }
      return {result};
    }
    if (StringUtil::StartsWith(internal_sql, "\\d ")) {
      auto table_name = std::string(internal_sql.cbegin() + 3, internal_sql.cend());
      auto table_info = catalog_->GetTable(table_name);
      if (table_info == nullptr) {
        return {"table not found"};
      }
      std::vector<std::string> info;
      info.emplace_back(fmt::format("name={} oid={}", table_info->name_, table_info->oid_));
      for (const auto &column : table_info->schema_.GetColumns()) {
        info.emplace_back(fmt::format("  {}", column.ToString()));
      }
      auto index_info = catalog_->GetTableIndexes(table_name);
      for (const auto *index : index_info) {
        info.emplace_back(fmt::format("index_name={} oid={}", index->name_, index->index_oid_));
        for (const auto &column : index->key_schema_.GetColumns()) {
          info.emplace_back(fmt::format("  {}", column.ToString()));
        }
      }
      return info;
    }
    if (sql == "\\help") {
      return {"Welcome to the BusTub shell!\n",
              "",
              "\\dt: show all tables",
              "\\d <table>: show info about a table",
              "\\help: show this message again",
              "",
              "BusTub shell currently only supports a small set of Postgres queries.",
              "We'll set up a doc describing the current status later.",
              "It will silently ignore some parts of the query, so it's normal that",
              "you'll get a wrong result when executing unsupported SQL queries.",
              "",
              "This shell will be able to run `create table` only after you have ",
              "completed the buffer pool manager. It will be able to execute SQL ",
              "queries after you have implemented necessary query executors.",
              "",
              "Use `explain` to see the execution plan of your query."};
    }
    throw Exception(fmt::format("unsupported internal command: {}", sql));
  }

  bustub::Binder binder(*catalog_);
  binder.ParseAndBindQuery(sql);
  std::vector<std::string> result = {};
  for (const auto &statement : binder.statements_) {
    switch (statement->type_) {
      case StatementType::CREATE_STATEMENT: {
        const auto &create_stmt = dynamic_cast<const CreateStatement &>(*statement);
        auto txn = transaction_manager_->Begin();
        auto info = catalog_->CreateTable(txn, create_stmt.table_, Schema(create_stmt.columns_));
        transaction_manager_->Commit(txn);
        delete txn;
        if (info == nullptr) {
          throw bustub::Exception("Failed to create table");
        }
        result.emplace_back(fmt::format("Table created with id = {}", info->oid_));
        continue;
      }
      case StatementType::INDEX_STATEMENT: {
        const auto &index_stmt = dynamic_cast<const IndexStatement &>(*statement);
        auto txn = transaction_manager_->Begin();

        std::vector<uint32_t> col_ids;
        for (const auto &col : index_stmt.cols_) {
          auto idx = index_stmt.table_->schema_.GetColIdx(col->col_name_.back());
          col_ids.push_back(idx);
          if (index_stmt.table_->schema_.GetColumn(idx).GetType() != TypeId::INTEGER) {
            throw NotImplementedException("only support creating index on integer column");
          }
        }
        if (col_ids.size() != 1) {
          throw NotImplementedException("only support creating index with exactly one column");
        }
        auto key_schema = Schema::CopySchema(&index_stmt.table_->schema_, col_ids);
        auto info = catalog_->CreateIndex<IntegerKeyType, IntegerValueType, IntegerComparatorType>(
            txn, index_stmt.index_name_, index_stmt.table_->table_, index_stmt.table_->schema_, key_schema, col_ids,
            INTEGER_SIZE, IntegerHashFunctionType{});
        transaction_manager_->Commit(txn);
        delete txn;
        if (info == nullptr) {
          throw bustub::Exception("Failed to create index");
        }
        result.emplace_back(fmt::format("Index created with id = {}", info->index_oid_));
        continue;
      }
      case StatementType::EXPLAIN_STATEMENT: {
        const auto &explain_stmt = dynamic_cast<const ExplainStatement &>(*statement);

        // Print binder result.
        if ((explain_stmt.options_ & ExplainOptions::BINDER) != 0) {
          result.emplace_back("=== BINDER ===");
          result.emplace_back(explain_stmt.statement_->ToString());
        }

        bustub::Planner planner(*catalog_);
        planner.PlanQuery(*explain_stmt.statement_);

        bool show_schema = (explain_stmt.options_ & ExplainOptions::SCHEMA) != 0;

        // Print planner result.
        if ((explain_stmt.options_ & ExplainOptions::PLANNER) != 0) {
          result.emplace_back("=== PLANNER ===");
          result.emplace_back(planner.plan_->ToString(show_schema));
        }

        // Print optimizer result.
        bustub::Optimizer optimizer(*catalog_);
        auto optimized_plan = optimizer.Optimize(planner.plan_);

        if ((explain_stmt.options_ & ExplainOptions::OPTIMIZER) != 0) {
          result.emplace_back("=== OPTIMIZER ===");
          result.emplace_back(optimized_plan->ToString(show_schema));
        }
        continue;
      }
      default:
        break;
    }

    // Plan the query.
    bustub::Planner planner(*catalog_);
    planner.PlanQuery(*statement);

    // Optimize the query.
    bustub::Optimizer optimizer(*catalog_);
    auto optimized_plan = optimizer.Optimize(planner.plan_);

    // Execute the query.
    auto txn = transaction_manager_->Begin();
    auto exec_ctx = MakeExecutorContext(txn);
    std::vector<Tuple> result_set{};
    execution_engine_->Execute(optimized_plan, &result_set, txn, exec_ctx.get());

    transaction_manager_->Commit(txn);
    delete txn;

    // Return the result set as a vector of string.
    auto schema = planner.plan_->OutputSchema();

    // Generate header for the result set.
    std::string header;
    for (const auto &column : schema.GetColumns()) {
      header += column.GetName();
      header += "\t";
    }
    result.emplace_back(std::move(header));

    // Transforming result set into strings.
    for (const auto &tuple : result_set) {
      std::string row;
      for (uint32_t i = 0; i < schema.GetColumnCount(); i++) {
        row += tuple.GetValue(&schema, i).ToString();
        row += "\t";
      }
      result.emplace_back(std::move(row));
    }
  }
  return result;
}

/**
 * FOR TEST ONLY. Generate test tables in this BusTub instance.
 * It's used in the shell to predefine some tables, as we don't support
 * create / drop table and insert for now. Should remove it in the future.
 */
void BustubInstance::GenerateTestTable() {
  auto txn = transaction_manager_->Begin();
  auto exec_ctx = MakeExecutorContext(txn);
  TableGenerator gen{exec_ctx.get()};
  gen.GenerateTestTables();

  transaction_manager_->Commit(txn);
  delete txn;
}

/**
 * FOR TEST ONLY. Generate test tables in this BusTub instance.
 * It's used in the shell to predefine some tables, as we don't support
 * create / drop table and insert for now. Should remove it in the future.
 */
void BustubInstance::GenerateMockTable() {
  // The actual content generated by mock scan executors are described in `mock_scan_executor.cpp`.
  auto txn = transaction_manager_->Begin();
  std::vector<Column> mock_table_1_columns{Column{"colA", TypeId::INTEGER}, {Column{"colB", TypeId::INTEGER}}};
  std::vector<Column> mock_table_2_columns{Column{"colC", TypeId::VARCHAR, 128},
                                           {Column{"colD", TypeId::VARCHAR, 128}}};
  std::vector<Column> mock_table_3_columns{Column{"colE", TypeId::INTEGER}, {Column{"colF", TypeId::VARCHAR, 128}}};

  auto mock_table_1_schema = Schema(mock_table_1_columns);
  catalog_->CreateTable(txn, "__mock_table_1", mock_table_1_schema, false);

  auto mock_table_2_schema = Schema(mock_table_2_columns);
  catalog_->CreateTable(txn, "__mock_table_2", mock_table_2_schema, false);

  auto mock_table_3_schema = Schema(mock_table_3_columns);
  catalog_->CreateTable(txn, "__mock_table_3", mock_table_3_schema, false);

  transaction_manager_->Commit(txn);
  delete txn;
}

BustubInstance::~BustubInstance() {
  if (enable_logging) {
    log_manager_->StopFlushThread();
  }
  delete execution_engine_;
  delete catalog_;
  delete checkpoint_manager_;
  delete log_manager_;
  delete buffer_pool_manager_;
  delete lock_manager_;
  delete transaction_manager_;
  delete disk_manager_;
}

}  // namespace bustub
