#include "common/bustub_instance.h"
#include "binder/binder.h"
#include "catalog/table_generator.h"
#include "fmt/format.h"
#include "recovery/log_manager.h"

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
    throw Exception(fmt::format("unsupported internal command: {}", sql));
  }

  bustub::Binder binder;
  binder.ParseAndBindQuery(sql, *catalog_);
  std::vector<std::string> result = {};
  for (auto &&statement : binder.statements_) {
    result.push_back(statement->ToString());
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
  delete catalog_;
  delete checkpoint_manager_;
  delete log_manager_;
  delete buffer_pool_manager_;
  delete lock_manager_;
  delete transaction_manager_;
  delete disk_manager_;
}

}  // namespace bustub
