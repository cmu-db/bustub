#pragma once

#include <algorithm>
#include <chrono>  // NOLINT
#include <cstdio>
#include <exception>
#include <functional>
#include <future>  // NOLINT
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "catalog/catalog.h"
#include "catalog/column.h"
#include "catalog/schema.h"
#include "common/bustub_instance.h"
#include "common/config.h"
#include "common/util/string_util.h"
#include "concurrency/transaction.h"
#include "concurrency/transaction_manager.h"
#include "concurrency/watermark.h"
#include "execution/execution_common.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/index/b_plus_tree.h"
#include "storage/table/tuple.h"
#include "test_util.h"  // NOLINT
#include "type/type.h"
#include "type/type_id.h"
#include "type/value_factory.h"

namespace bustub {

auto Int(uint32_t data) -> Value { return ValueFactory::GetIntegerValue(data); }

auto IntNull() -> Value { return ValueFactory::GetNullValueByType(TypeId::INTEGER); }

auto Double(double data) -> Value { return ValueFactory::GetDecimalValue(data); }

auto DoubleNull() -> Value { return ValueFactory::GetNullValueByType(TypeId::DECIMAL); }

auto Bool(bool data) -> Value { return ValueFactory::GetBooleanValue(data); }

auto BoolNull() -> Value { return ValueFactory::GetNullValueByType(TypeId::BOOLEAN); }

auto CompareValue(const Value &v1, const Value &v2) -> bool {
  return v1.CompareEquals(v2) == CmpBool::CmpTrue || (v1.IsNull() && v2.IsNull());
}

void VerifyTuple(const Schema *schema, const Tuple &tuple, const std::vector<Value> &values) {
  for (uint32_t i = 0; i < schema->GetColumnCount(); i++) {
    auto actual_val = tuple.GetValue(schema, i);
    auto &expected_val = values[i];
    if (!CompareValue(actual_val, expected_val)) {
      std::cerr << "tuple data mismatch: got " << tuple.ToString(schema) << "; at column " << i << ", actual value "
                << actual_val.ToString() << " != expected value " << expected_val.ToString() << std::endl;
      std::terminate();
    }
  }
}

template <typename T>
auto ResultVecToString(const std::vector<std::vector<T>> &result) -> std::string {
  std::vector<std::string> rows;
  for (const auto &row : result) {
    if (row.empty()) {
      rows.emplace_back("<empty row>");
    } else {
      rows.emplace_back(fmt::format("{}", fmt::join(row, ", ")));
    }
  }
  if (result.empty()) {
    rows.emplace_back("<empty table>");
  }
  return fmt::format("{}", fmt::join(rows, "\n"));
}

template <typename T>
auto ResultVecToVecString(const std::vector<std::vector<T>> &result) -> std::vector<std::vector<std::string>> {
  std::vector<std::vector<std::string>> rows;
  for (const auto &row : result) {
    rows.emplace_back();
    for (const auto &col : row) {
      rows.back().push_back(fmt::format("{}", col));
    }
  }
  return rows;
}

template <typename T>
auto VecToVecString(const std::vector<T> &result) -> std::vector<std::string> {
  std::vector<std::string> rows;
  for (const auto &col : result) {
    rows.emplace_back(fmt::format("{}", col));
  }
  return rows;
}

template <typename T>
auto ExpectResult(const std::vector<std::vector<std::string>> &actual_result,
                  const std::vector<std::vector<T>> &expected_result, bool show_result, bool newline = true) -> bool {
  auto actual_result_rows = ResultVecToVecString(actual_result);
  auto expected_result_rows = ResultVecToVecString(expected_result);
  std::sort(actual_result_rows.begin(), actual_result_rows.end());
  std::sort(expected_result_rows.begin(), expected_result_rows.end());
  if (actual_result_rows != expected_result_rows) {
    auto expected_result_str = ResultVecToString(expected_result);
    auto actual_result_str = ResultVecToString(actual_result);
    if (show_result) {
      if (!newline) {
        fmt::println(stderr, "-- error :(");
      }
      fmt::print(stderr, "ERROR: result mismatch\n--- EXPECTED ---\n{}\n\n--- ACTUAL ---\n{}\n\n", expected_result_str,
                 actual_result_str);
    } else {
      if (!newline) {
        fmt::println(stderr, "-- error :(");
      }
      fmt::print(stderr, "ERROR: result mismatch\n--- EXPECTED ---\n<hidden>\n\n--- ACTUAL ---\n{}\n\n",
                 actual_result_str);
    }
  } else {
    if (!newline) {
      fmt::print(stderr, "-");
    }
    fmt::print(stderr, "- pass check :)\n");
  }
  return actual_result_rows == expected_result_rows;
}

template <typename T>
void Query(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn, const std::string &query,
           const std::vector<std::vector<T>> &expected_rows, bool show_result) {
  std::stringstream ss;
  auto writer = bustub::StringVectorWriter();
  fmt::print(stderr, "- query var={} id={} status={} read_ts={} query=\"{}\" ", txn_var_name,
             txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs(), query);
  if (txn->GetTransactionState() != TransactionState::RUNNING) {
    fmt::println(stderr, "txn not running");
    std::terminate();
  }
  if (!instance.ExecuteSqlTxn(query, writer, txn)) {
    std::cerr << "failed to execute sql" << std::endl;
    std::terminate();
  }
  if (!ExpectResult(writer.values_, expected_rows, show_result, false)) {
    std::terminate();
  }
}

template <typename T>
void QueryHideResult(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn,
                     const std::string &query, const std::vector<std::vector<T>> &expected_rows) {
  Query(instance, txn_var_name, txn, query, expected_rows, false);
}

template <typename T>
void QueryShowResult(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn,
                     const std::string &query, const std::vector<std::vector<T>> &expected_rows) {
  Query(instance, txn_var_name, txn, query, expected_rows, true);
}

void CheckUndoLogNum(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn, size_t expected_num) {
  fmt::println(stderr, "- check_undo_log var={} id={} status={} read_ts={}", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs());
  auto undo_log_num = txn->GetUndoLogNum();
  if (undo_log_num != expected_num) {
    fmt::println(stderr, "Error: expected to have {} undo logs in txn, found {}", expected_num, undo_log_num);
    std::terminate();
  }
}

void CheckUndoLogColumn(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn,
                        size_t expected_columns) {
  fmt::println(stderr, "- check_undo_log var={} id={} status={} read_ts={}", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs());
  auto undo_log_num = txn->GetUndoLogNum();
  if (undo_log_num != 1) {
    fmt::println(stderr, "Error: expected to have {} undo logs in txn, found {}", 1, undo_log_num);
    std::terminate();
  }
  auto undo_log = txn->GetUndoLog(0);
  size_t cnt =
      std::count_if(undo_log.modified_fields_.begin(), undo_log.modified_fields_.end(), [](const auto x) { return x; });
  if (cnt != expected_columns) {
    fmt::println(stderr, "Error: expected undo log to have {} columns, found {} (modified_fields=[{}])",
                 expected_columns, cnt, fmt::join(undo_log.modified_fields_, ","));
    std::terminate();
  }
}

#define WithTxn(txn, func)                             \
  {                                                    \
    [[maybe_unused]] const std::string &_var = (#txn); \
    [[maybe_unused]] Transaction *_txn = txn;          \
    func;                                              \
  }

void Execute(BustubInstance &instance, const std::string &query, bool log = true) {
  if (log) {
    fmt::println(stderr, "- execute sql=\"{}\"", query);
  }
  NoopWriter writer;
  if (!instance.ExecuteSql(query, writer)) {
    std::cerr << "failed to execute sql" << std::endl;
    std::terminate();
  }
}

auto TableHeapEntry(BustubInstance &instance, TableInfo *table_info) -> size_t {
  auto table_heap = table_info->table_.get();
  auto table_iter = table_heap->MakeEagerIterator();
  size_t cnt = 0;
  while (!table_iter.IsEnd()) {
    ++table_iter;
    cnt++;
  }
  return cnt;
}

void TableHeapEntryNoMoreThan(BustubInstance &instance, TableInfo *table_info, size_t upper_limit) {
  fmt::print(stderr, "- verify table heap");
  auto cnt = TableHeapEntry(instance, table_info);
  if (cnt > upper_limit) {
    fmt::println(stderr, " -- error: expect table heap to contain at most {} elements, found {}", upper_limit, cnt);
    std::terminate();
  }
  fmt::println(stderr, "");
}

void ExecuteTxn(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn, const std::string &query) {
  fmt::println(stderr, "- execute var={} id={} status={} read_ts={} sql=\"{}\"", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs(), query);
  if (txn->GetTransactionState() != TransactionState::RUNNING) {
    fmt::println(stderr, "txn not running");
    std::terminate();
  }
  NoopWriter writer;
  if (!instance.ExecuteSqlTxn(query, writer, txn)) {
    std::cerr << "failed to execute sql" << std::endl;
    std::terminate();
  }
}

auto BeginTxn(BustubInstance &instance, const std::string &txn_var_name,
              IsolationLevel iso_lvl = IsolationLevel::SNAPSHOT_ISOLATION) -> Transaction * {
  auto txn = instance.txn_manager_->Begin(iso_lvl);
  fmt::println(stderr, "- txn_begin var={} id={} status={} read_ts={} iso_lvl={}", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs(),
               txn->GetIsolationLevel());
  return txn;
}

auto BeginTxnSerializable(BustubInstance &instance, const std::string &txn_var_name) -> Transaction * {
  return BeginTxn(instance, txn_var_name, IsolationLevel::SERIALIZABLE);
}

const bool EXPECT_FAIL = true;

auto CommitTxn(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn, bool expect_fail = false) {
  if (txn->GetTransactionState() != TransactionState::RUNNING) {
    fmt::println(stderr, "txn not running");
    std::terminate();
  }
  auto res = instance.txn_manager_->Commit(txn);
  if (!expect_fail) {
    if (!res) {
      fmt::println(stderr, "failed to commit txn: var={} id={}", txn_var_name, txn->GetTransactionId());
      std::terminate();
    }
    if (txn->GetTransactionState() != TransactionState::COMMITTED) {
      fmt::println(stderr, "should set to committed state var={} id={}", txn_var_name, txn->GetTransactionId());
      std::terminate();
    }
    fmt::println(stderr, "- txn_commit var={} id={} status={} read_ts={} commit_ts={}", txn_var_name,
                 txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs(),
                 txn->GetCommitTs());
    return;
  }
  if (res) {
    fmt::println(stderr, "expect txn fail to commit, but committed: var={} id={}", txn_var_name,
                 txn->GetTransactionId());
    std::terminate();
  }
  if (txn->GetTransactionState() != TransactionState::ABORTED) {
    fmt::println(stderr, "should set to aborted state var={} id={}", txn_var_name, txn->GetTransactionId());
    std::terminate();
  }
  fmt::println(stderr, "- txn_commit_fail var={} id={} status={} read_ts={}", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs());
}

auto AbortTxn(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn) {
  if (txn->GetTransactionState() != TransactionState::RUNNING &&
      txn->GetTransactionState() != TransactionState::TAINTED) {
    fmt::println(stderr, "txn not running / tainted");
    std::terminate();
  }
  instance.txn_manager_->Abort(txn);
  if (txn->GetTransactionState() != TransactionState::ABORTED) {
    fmt::println(stderr, "should set to aborted state var={} id={}", txn_var_name, txn->GetTransactionId());
    std::terminate();
  }
  fmt::println(stderr, "- txn_abort var={} id={} status={} read_ts={}", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs());
}

auto CheckTainted(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn) {
  if (txn->GetTransactionState() != TransactionState::TAINTED) {
    fmt::println(stderr, "should set to tainted state var={} id={}", txn_var_name, txn->GetTransactionId());
    std::terminate();
  }
  fmt::println(stderr, "- txn_tainted var={} id={} status={} read_ts={}", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs());
}

auto CommitTaintedTxn(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn) {
  if (instance.txn_manager_->Commit(txn)) {
    fmt::println(stderr, "should not commit var={} id={}", txn_var_name, txn->GetTransactionId());
    std::terminate();
  }
  if (txn->GetTransactionState() != TransactionState::TAINTED) {
    fmt::println(stderr, "should set to tainted state var={} id={}", txn_var_name, txn->GetTransactionId());
    std::terminate();
  }
  fmt::println(stderr, "- txn_committed_tainted var={} id={} status={} read_ts={}", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs());
}

void ExecuteTxnTainted(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn,
                       const std::string &query) {
  fmt::println(stderr, "- execute var={} id={} status={} read_ts={} sql=\"{}\"", txn_var_name,
               txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs(), query);
  NoopWriter writer;
  if (instance.ExecuteSqlTxn(query, writer, txn)) {
    std::cerr << "sql should not execute successfully -- there should be a write-write conflict" << std::endl;
    std::terminate();
  }
  CheckTainted(instance, txn_var_name, txn);
}

void GarbageCollection(BustubInstance &instance) {
  fmt::println(stderr, "- garbage_collection");
  instance.txn_manager_->GarbageCollection();
}

void EnsureTxnGCed(BustubInstance &instance, const std::string &txn_var_name, txn_id_t txn_id) {
  fmt::println(stderr, "- ensure_txn_gc_ed var={} id={} watermark={}", txn_var_name, txn_id ^ TXN_START_ID,
               instance.txn_manager_->GetWatermark());
  const auto &txn_map = instance.txn_manager_->txn_map_;
  if (txn_map.find(txn_id) != txn_map.end()) {
    std::cerr << "txn not garbage collected" << std::endl;
    std::terminate();
  }
}

void EnsureTxnExists(BustubInstance &instance, const std::string &txn_var_name, txn_id_t txn_id) {
  fmt::println(stderr, "- ensure_txn_exists var={} id={} watermark={}", txn_var_name, txn_id ^ TXN_START_ID,
               instance.txn_manager_->GetWatermark());
  const auto &txn_map = instance.txn_manager_->txn_map_;
  if (txn_map.find(txn_id) == txn_map.end()) {
    std::cerr << "txn not exist" << std::endl;
    std::terminate();
  }
}

void BumpCommitTs(BustubInstance &instance, int by = 1) {
  auto before_commit_ts = instance.txn_manager_->last_commit_ts_.load();
  for (int i = 0; i < by; i++) {
    auto txn = instance.txn_manager_->Begin();
    instance.txn_manager_->Commit(txn);
  }
  auto after_commit_ts = instance.txn_manager_->last_commit_ts_.load();
  fmt::println(stderr, "- bump_commit_ts from={} to={} watermark={}", before_commit_ts, after_commit_ts,
               instance.txn_manager_->GetWatermark());
}

void EnsureIndexScan(BustubInstance &instance) {
  global_disable_execution_exception_print.store(true);
  fmt::println(stderr, "- pre-test index validation");
  NoopWriter writer;
  auto res = instance.ExecuteSql("CREATE TABLE pk_test_table_n(pk int primary key)", writer);
  if (!res) {
    std::terminate();
  }
  res = instance.ExecuteSql("set force_optimizer_starter_rule=yes", writer);
  if (!res) {
    std::terminate();
  }

  {
    std::stringstream ss;
    SimpleStreamWriter writer_ss(ss);
    auto *txn = instance.txn_manager_->Begin();
    res = instance.ExecuteSqlTxn("EXPLAIN SELECT * FROM pk_test_table_n WHERE pk = 1", writer_ss, txn);
    if (!StringUtil::Contains(ss.str(), "IndexScan")) {
      fmt::println("index scan not found in plan:\n{}\n", ss.str());
      std::terminate();
    }
  }

  global_disable_execution_exception_print.store(false);
}

template <typename T>
void QueryIndex(BustubInstance &instance, const std::string &txn_var_name, Transaction *txn, const std::string &query,
                const std::string &pk_column, const std::vector<T> &expected_pk,
                const std::vector<std::vector<T>> &expected_rows) {
  assert(expected_pk.size() == expected_rows.size());
  fmt::print(stderr, "- query_index var={} id={} status={} read_ts={} query=\"{}\" pk={}", txn_var_name,
             txn->GetTransactionIdHumanReadable(), txn->GetTransactionState(), txn->GetReadTs(), query,
             fmt::join(expected_pk, ","));
  if (txn->GetTransactionState() != TransactionState::RUNNING) {
    fmt::println(stderr, "txn not running");
    std::terminate();
  }
  for (size_t i = 0; i < expected_pk.size(); i++) {
    StringVectorWriter writer;
    auto res = instance.ExecuteSqlTxn(fmt::format("{} WHERE {} = {}", query, pk_column, expected_pk[i]), writer, txn);
    if (!res) {
      fmt::println("failed to execute query when {} = {}", pk_column, expected_pk[i]);
      std::terminate();
    }
    if (expected_rows[i].empty()) {
      if (!writer.values_.empty()) {
        fmt::println("ERROR: expect {} = {} to have 0 rows, found ({})", pk_column, expected_pk[i],
                     fmt::join(writer.values_[0], ","));
        std::terminate();
      }
    } else {
      if (writer.values_.size() != 1) {
        fmt::println("ERROR: expect {} = {} to have 1 row, found {} rows", pk_column, expected_pk[i],
                     writer.values_.size());
        std::terminate();
      }
      if (writer.values_[0] != VecToVecString(expected_rows[i])) {
        fmt::println("ERROR: expect {} = {} to be ({}), found ({})", pk_column, expected_pk[i],
                     fmt::join(expected_rows[0], ","), fmt::join(writer.values_[0], ","));
        std::terminate();
      }
    }
  }
  fmt::println(" -- pass check :)");
}

using IntResult = std::vector<std::vector<int>>;

}  // namespace bustub
