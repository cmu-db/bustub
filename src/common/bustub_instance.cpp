#include "common/bustub_instance.h"
#include "binder/binder.h"
#include "buffer/buffer_pool_manager_instance.h"
#include "catalog/table_generator.h"
#include "concurrency/lock_manager.h"
#include "execution/execution_engine.h"
#include "execution/executor_context.h"
#include "fmt/format.h"
#include "planner/planner.h"
#include "recovery/checkpoint_manager.h"
#include "recovery/log_manager.h"
#include "storage/disk/disk_manager.h"

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

  buffer_pool_manager_ = new BufferPoolManagerInstance(BUFFER_POOL_SIZE, disk_manager_, log_manager_);

  // Transaction (txn) related.
  lock_manager_ = new LockManager();
  transaction_manager_ = new TransactionManager(lock_manager_, log_manager_);

  // Checkpoint related.
  checkpoint_manager_ = new CheckpointManager(transaction_manager_, log_manager_, buffer_pool_manager_);

  // Catalog.
  catalog_ = new Catalog(buffer_pool_manager_, nullptr, nullptr);

  // execution engine
  execution_engine_ = new ExecutionEngine(buffer_pool_manager_, transaction_manager_, catalog_);
}

auto BustubInstance::ExecuteSql(const std::string &sql) -> std::vector<std::string> {
  if (!sql.empty() && sql[0] == '\\') {
    // Internal meta-commands, like in `psql`.
    if (sql == "\\dt") {
      std::string result;
      auto table_names = catalog_->GetTableNames();
      for (const auto &name : table_names) {
        result += name;
        result += "\n";
      }
      return {result};
    }
    if (sql == "\\help") {
      return {"help!\n"};
    }
    throw Exception(fmt::format("unsupported internal command: {}", sql));
  }

  bustub::Binder binder;
  binder.ParseAndBindQuery(sql, *catalog_);
  std::vector<std::string> result = {};
  for (const auto &statement : binder.statements_) {
    // Bind the query.
    std::cerr << "=== BINDER ===" << std::endl;
    std::cerr << statement->ToString() << std::endl;

    // Plan the query.
    bustub::Planner planner(*catalog_);
    planner.PlanQuery(*statement);
    std::cerr << "=== PLANNER ===" << std::endl;
    std::cerr << planner.plan_->ToString() << std::endl;

    // Execute the query.
    auto txn = transaction_manager_->Begin();
    auto exec_ctx = MakeExecutorContext(txn);
    std::vector<Tuple> result_set{};
    execution_engine_->Execute(planner.plan_.get(), &result_set, txn, exec_ctx.get());
    delete txn;

    // Return the result set as a vector of string.
    auto schema = planner.plan_->OutputSchema();
    for (const auto &tuple : result_set) {
      std::string row;
      for (uint32_t i = 0; i < schema->GetColumnCount(); i++) {
        row += tuple.GetValue(schema, i).ToString();
        row += " ";
      }
      result.push_back(row);
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
