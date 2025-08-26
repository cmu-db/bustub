//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bustub_ddl.cpp
//
// Identification: src/common/bustub_ddl.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

// DDL (Data Definition Language) statement handling in BusTub, including create table, create index, and set/show
// variable.

#include <optional>
#include <shared_mutex>
#include <string>
#include <tuple>

#include "binder/binder.h"
#include "binder/bound_expression.h"
#include "binder/bound_statement.h"
#include "binder/statement/create_statement.h"
#include "binder/statement/explain_statement.h"
#include "binder/statement/index_statement.h"
#include "binder/statement/select_statement.h"
#include "binder/statement/set_show_statement.h"
#include "buffer/buffer_pool_manager.h"
#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "catalog/table_generator.h"
#include "common/bustub_instance.h"
#include "common/enums/statement_type.h"
#include "common/exception.h"
#include "common/macros.h"
#include "common/util/string_util.h"
#include "concurrency/lock_manager.h"
#include "concurrency/transaction.h"
#include "execution/execution_engine.h"
#include "execution/executor_context.h"
#include "execution/executors/mock_scan_executor.h"
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
#include "type/value_factory.h"

namespace bustub {

void BusTubInstance::HandleCreateStatement(Transaction *txn, const CreateStatement &stmt, ResultWriter &writer) {
  std::unique_lock<std::shared_mutex> l(catalog_lock_);
  auto info = catalog_->CreateTable(txn, stmt.table_, Schema(stmt.columns_));
  std::shared_ptr<IndexInfo> index = nullptr;
  if (!stmt.primary_key_.empty()) {
    std::vector<uint32_t> col_ids;
    for (const auto &col : stmt.primary_key_) {
      auto idx = info->schema_.GetColIdx(col);
      col_ids.push_back(idx);
      if (info->schema_.GetColumn(idx).GetType() != TypeId::INTEGER) {
        throw NotImplementedException("only support creating index on integer column");
      }
    }
    auto key_schema = Schema::CopySchema(&info->schema_, col_ids);

    // TODO(spring2023): If you want to support composite index key for leaderboard optimization, remove this assertion
    // and create index with different key type that can hold multiple keys based on number of index columns.
    //
    // You can also create clustered index that directly stores value inside the index by modifying the value type.

    if (col_ids.empty() || col_ids.size() > 2) {
      throw NotImplementedException("only support creating index with exactly one or two columns");
    }

    index = catalog_->CreateIndex<IntegerKeyType, IntegerValueType, IntegerComparatorType>(
        txn, stmt.table_ + "_pk", stmt.table_, info->schema_, key_schema, col_ids, TWO_INTEGER_SIZE,
        IntegerHashFunctionType{}, true);
  }
  l.unlock();

  if (info == nullptr) {
    throw bustub::Exception("Failed to create table");
  }

  if (index != nullptr) {
    WriteOneCell(fmt::format("Table created with id = {}, Primary key index created with id = {}", info->oid_,
                             index->index_oid_),
                 writer);
  } else {
    WriteOneCell(fmt::format("Table created with id = {}", info->oid_), writer);
  }
}

void BusTubInstance::HandleIndexStatement(Transaction *txn, const IndexStatement &stmt, ResultWriter &writer) {
  std::vector<uint32_t> col_ids;
  for (const auto &col : stmt.cols_) {
    auto idx = stmt.table_->schema_.GetColIdx(col->col_name_.back());
    col_ids.push_back(idx);
    if (stmt.table_->schema_.GetColumn(idx).GetType() != TypeId::INTEGER) {
      throw NotImplementedException("only support creating index on integer column");
    }
  }
  auto key_schema = Schema::CopySchema(&stmt.table_->schema_, col_ids);

  // TODO(spring2023): If you want to support composite index key for leaderboard optimization, remove this assertion
  // and create index with different key type that can hold multiple keys based on number of index columns.
  //
  // You can also create clustered index that directly stores value inside the index by modifying the value type.

  if (col_ids.empty() || col_ids.size() > 2) {
    throw NotImplementedException("only support creating index with exactly one or two columns");
  }

  std::unique_lock<std::shared_mutex> l(catalog_lock_);
  std::shared_ptr<IndexInfo> info = nullptr;

  if (stmt.index_type_.empty()) {
    info = catalog_->CreateIndex<IntegerKeyType, IntegerValueType, IntegerComparatorType>(
        txn, stmt.index_name_, stmt.table_->table_, stmt.table_->schema_, key_schema, col_ids, TWO_INTEGER_SIZE,
        IntegerHashFunctionType{}, false);  // create default index
  } else if (stmt.index_type_ == "hash") {
    info = catalog_->CreateIndex<IntegerKeyType, IntegerValueType, IntegerComparatorType>(
        txn, stmt.index_name_, stmt.table_->table_, stmt.table_->schema_, key_schema, col_ids, TWO_INTEGER_SIZE,
        IntegerHashFunctionType{}, false, IndexType::HashTableIndex);
  } else if (stmt.index_type_ == "bplustree") {
    info = catalog_->CreateIndex<IntegerKeyType, IntegerValueType, IntegerComparatorType>(
        txn, stmt.index_name_, stmt.table_->table_, stmt.table_->schema_, key_schema, col_ids, TWO_INTEGER_SIZE,
        IntegerHashFunctionType{}, false, IndexType::BPlusTreeIndex);
  } else if (stmt.index_type_ == "stl_ordered") {
    info = catalog_->CreateIndex<IntegerKeyType, IntegerValueType, IntegerComparatorType>(
        txn, stmt.index_name_, stmt.table_->table_, stmt.table_->schema_, key_schema, col_ids, TWO_INTEGER_SIZE,
        IntegerHashFunctionType{}, false, IndexType::STLOrderedIndex);
  } else if (stmt.index_type_ == "stl_unordered") {
    info = catalog_->CreateIndex<IntegerKeyType, IntegerValueType, IntegerComparatorType>(
        txn, stmt.index_name_, stmt.table_->table_, stmt.table_->schema_, key_schema, col_ids, TWO_INTEGER_SIZE,
        IntegerHashFunctionType{}, false, IndexType::STLUnorderedIndex);
  } else {
    UNIMPLEMENTED("unsupported index type " + stmt.index_type_);
  }
  l.unlock();

  if (info == nullptr) {
    throw bustub::Exception("Failed to create index");
  }
  WriteOneCell(fmt::format("Index created with id = {} with type = {}", info->index_oid_, info->index_type_), writer);
}

void BusTubInstance::HandleExplainStatement(Transaction *txn, const ExplainStatement &stmt, ResultWriter &writer) {
  std::string output;

  // Print binder result.
  if ((stmt.options_ & ExplainOptions::BINDER) != 0) {
    output += "=== BINDER ===";
    output += "\n";
    output += stmt.statement_->ToString();
    output += "\n";
  }

  std::shared_lock<std::shared_mutex> l(catalog_lock_);

  bustub::Planner planner(*catalog_);
  planner.PlanQuery(*stmt.statement_);

  bool show_schema = (stmt.options_ & ExplainOptions::SCHEMA) != 0;

  // Print planner result.
  if ((stmt.options_ & ExplainOptions::PLANNER) != 0) {
    output += "=== PLANNER ===";
    output += "\n";
    output += planner.plan_->ToString(show_schema);
    output += "\n";
  }

  // Print optimizer result.
  bustub::Optimizer optimizer(*catalog_, IsForceStarterRule());
  auto optimized_plan = optimizer.Optimize(planner.plan_);

  l.unlock();

  if ((stmt.options_ & ExplainOptions::OPTIMIZER) != 0) {
    output += "=== OPTIMIZER ===";
    output += "\n";
    output += optimized_plan->ToString(show_schema);
    output += "\n";
  }

  WriteOneCell(output, writer);
}

void BusTubInstance::HandleVariableShowStatement(Transaction *txn, const VariableShowStatement &stmt,
                                                 ResultWriter &writer) {
  auto content = GetSessionVariable(stmt.variable_);
  WriteOneCell(fmt::format("{}={}", stmt.variable_, content), writer);
}

void BusTubInstance::HandleVariableSetStatement(Transaction *txn, const VariableSetStatement &stmt,
                                                ResultWriter &writer) {
  session_variables_[stmt.variable_] = stmt.value_;
}

void BusTubInstance::HandleTxnStatement(Transaction *txn, const TransactionStatement &stmt, ResultWriter &writer) {
  if (managed_txn_mode_ && current_txn_ != nullptr) {
    BUSTUB_ASSERT(current_txn_ == txn, "txn mismatched??");
  }
  auto dump_current_txn = [&](const std::string &prefix) {
    writer.OneCell(fmt::format("{}txn_id={} txn_real_id={} read_ts={} commit_ts={} status={} iso_lvl={}", prefix,
                               current_txn_->GetTransactionIdHumanReadable(), current_txn_->GetTransactionId(),
                               current_txn_->GetReadTs(), current_txn_->GetCommitTs(),
                               current_txn_->GetTransactionState(), current_txn_->GetIsolationLevel()));
  };
  if (txn == nullptr) {
    writer.OneCell("commit / rollback can only be used with txn");
    return;
  }
  if (stmt.type_ == "begin") {
    if (!managed_txn_mode_) {
      writer.OneCell("begin statement is only supported in managed txn mode, please use bustub-shell");
      return;
    }
    bool txn_activated = current_txn_ != nullptr;
    auto iso_lvl = StringUtil::Lower(GetSessionVariable("global_isolation_level"));
    if (iso_lvl == "serializable") {
      current_txn_ = txn_manager_->Begin(IsolationLevel::SERIALIZABLE);
    } else if (iso_lvl == "snapshot_isolation" || iso_lvl.empty()) {
      current_txn_ = txn_manager_->Begin(IsolationLevel::SNAPSHOT_ISOLATION);
    } else {
      throw Exception("unsupported global_isolation_level");
    }
    dump_current_txn(txn_activated ? "pause current txn and begin new txn " : "begin txn ");
    return;
  }
  if (stmt.type_ == "commit") {
    auto res = txn_manager_->Commit(txn);
    if (res) {
      writer.OneCell(fmt::format("txn committed, txn_id={}, status={}, read_ts={}, commit_ts={}",
                                 txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs(),
                                 txn->GetCommitTs()));
    } else {
      writer.OneCell(fmt::format("txn failed to commit, txn_id={}, status={}, read_ts={}, commit_ts={}",
                                 txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs(),
                                 txn->GetCommitTs()));
    }
    current_txn_ = nullptr;
    return;
  }
  if (stmt.type_ == "abort") {
    txn_manager_->Abort(txn);
    writer.OneCell(fmt::format("txn aborted, txn_id={}, status={}, read_ts={}", txn->GetTransactionIdHumanReadable(),
                               txn->GetTransactionState(), txn->GetReadTs()));
    current_txn_ = nullptr;
    return;
  }
}
}  // namespace bustub
