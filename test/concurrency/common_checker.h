#pragma once

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <atomic>
#include <chrono>  // NOLINT
#include <cstdio>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>  //NOLINT
#include <utility>
#include <vector>

#include "common/bustub_instance.h"
#include "concurrency/transaction_manager.h"

namespace bustub {

auto Insert(Transaction *txn, BustubInstance &instance, int v1) -> void {
  std::stringstream ss;
  auto writer = bustub::SimpleStreamWriter(ss, true, ",");
  fmt::print(stderr, "insert data with v1 = {} in txn {} {}\n", v1, txn->GetTransactionId(), txn->GetIsolationLevel());
  std::string sql = fmt::format("INSERT INTO t1 VALUES ({}, 1), ({}, 2), ({}, 3)", v1, v1, v1);
  instance.ExecuteSqlTxn(sql, writer, txn);
  ASSERT_EQ(ss.str(), "3,\n");
}

auto Delete(Transaction *txn, BustubInstance &instance, int v1) -> void {
  std::stringstream ss;
  auto writer = bustub::SimpleStreamWriter(ss, true, ",");
  fmt::print(stderr, "delete data with v1 = {} in txn {} {}\n", v1, txn->GetTransactionId(), txn->GetIsolationLevel());
  std::string sql = fmt::format("DELETE FROM t1 WHERE v1 = {}", v1);
  instance.ExecuteSqlTxn(sql, writer, txn);
  ASSERT_EQ(ss.str(), "3,\n");
}

auto ExpectResult(const std::string &actual_result, const std::string &expected_result) -> bool {
  auto actual_result_rows = bustub::StringUtil::Split(actual_result, '\n');
  auto expected_result_rows = bustub::StringUtil::Split(expected_result, '\n');
  std::sort(actual_result_rows.begin(), actual_result_rows.end());
  std::sort(expected_result_rows.begin(), expected_result_rows.end());
  if (actual_result_rows != expected_result_rows) {
    fmt::print(stderr, "ERROR: result mismatch\n--- EXPECTED ---\n{}\n\n--- ACTUAL ---\n{}\n", expected_result,
               actual_result);
  } else {
    fmt::print(stderr, "pass check\n");
  }
  return actual_result_rows == expected_result_rows;
}

auto Scan(Transaction *txn, BustubInstance &instance, const std::vector<int> &v1) -> void {
  std::stringstream ss;
  auto writer = bustub::SimpleStreamWriter(ss, true, ",");
  fmt::print(stderr, "scan data expect v1 in range {} in txn {} {}\n", v1, txn->GetTransactionId(),
             txn->GetIsolationLevel());
  std::string sql = fmt::format("SELECT * FROM t1");
  instance.ExecuteSqlTxn(sql, writer, txn);
  std::string expected_result;
  for (auto v : v1) {
    for (size_t i = 1; i <= 3; i++) {
      expected_result += fmt::format("{},{},\n", v, i);
    }
  }
  if (!ExpectResult(ss.str(), expected_result)) {
    throw std::runtime_error("result mismatch");
  }
}

void Commit(BustubInstance &instance, Transaction *&txn) {
  fmt::print(stderr, "commit txn {} in {}\n", txn->GetTransactionId(), txn->GetIsolationLevel());
  instance.txn_manager_->Commit(txn);
  delete txn;
  txn = nullptr;
}

void Abort(BustubInstance &instance, Transaction *&txn) {
  fmt::print(stderr, "abort txn {} in {}\n", txn->GetTransactionId(), txn->GetIsolationLevel());
  instance.txn_manager_->Abort(txn);
  delete txn;
  txn = nullptr;
}

auto Begin(BustubInstance &instance, IsolationLevel level) -> Transaction * {
  auto txn = instance.txn_manager_->Begin(nullptr, level);
  fmt::print(stderr, "start txn {} in {}\n", txn->GetTransactionId(), txn->GetIsolationLevel());
  return txn;
}

auto GetDbForVisibilityTest(const std::string &name) -> std::shared_ptr<BustubInstance> {
  auto instance = std::make_unique<BustubInstance>();
  auto writer = bustub::SimpleStreamWriter(std::cout, true);
  fmt::print(stderr, "--- TEST CASE {} ---\n", name);
  fmt::print(stderr, "prepare\n");
  instance->ExecuteSql("CREATE TABLE t1(v1 int, v2 int);", writer);
  instance->ExecuteSql("INSERT INTO t1 VALUES (233, 1), (233, 2), (233, 3), (234, 1), (234, 2), (234, 3)", writer);
  return instance;
}

auto GetDbForCommitAbortTest(const std::string &name) -> std::shared_ptr<BustubInstance> {
  auto instance = std::make_unique<BustubInstance>();
  auto writer = bustub::SimpleStreamWriter(std::cout, true);
  fmt::print(stderr, "--- TEST CASE {} ---\n", name);
  fmt::print(stderr, "prepare\n");
  instance->ExecuteSql("CREATE TABLE t1(v1 int, v2 int);", writer);
  instance->ExecuteSql("INSERT INTO t1 VALUES (233, 1), (233, 2), (233, 3), (234, 1), (234, 2), (234, 3)", writer);
  return instance;
}

enum class ExpectedOutcome {
  DirtyRead,
  Read,
  BlockOnRead,
  BlockOnWrite,
};

auto GetDbForIsolationTest() -> std::shared_ptr<BustubInstance> {
  auto instance = std::make_unique<BustubInstance>();
  auto writer = bustub::SimpleStreamWriter(std::cout, true);
  fmt::print(stderr, "0: prepare\n");
  // in case you implemented scan filter pushdown... disable it by forcing starter rule!
  instance->ExecuteSql("set force_optimizer_starter_rule=yes;", writer);
  fmt::print(stderr, "0.1: create table\n");
  instance->ExecuteSql("CREATE TABLE t1(v1 int, v2 int);", writer);
  fmt::print(stderr, "0.2: insert initial value\n");
  instance->ExecuteSql("INSERT INTO t1 VALUES (1, 2), (1, 3), (1, 4), (2, 2), (2, 3), (2, 4);", writer);
  return instance;
}

auto StartReadTxn(IsolationLevel read_txn_level, BustubInstance &instance, bool read_before_write) -> Transaction * {
  fmt::print(stderr, "1: read txn: {}\n", read_txn_level);
  auto txn_r = instance.txn_manager_->Begin(nullptr, read_txn_level);
  if (read_before_write) {
    fmt::print(stderr, "1.1: initial read\n");
    std::stringstream result;
    auto writer = bustub::SimpleStreamWriter(result, true, ",");
    instance.ExecuteSqlTxn("SELECT * FROM t1;", writer, txn_r);
    fmt::print(stderr, "1.2: R locks\n");
    for (const auto &[t, v] : *txn_r->GetSharedRowLockSet()) {
      for (const auto &l : v) {
        std::cout << "table: " << t << " " << l;
      }
    }
    auto is_eq = ExpectResult(result.str(), "1,2,\n1,3,\n1,4,\n2,2,\n2,3,\n2,4,\n");
    EXPECT_TRUE(is_eq);
    fmt::print(stderr, "1.3: read done\n");
    if (!is_eq) {
      fmt::print(stderr,
                 "ERROR: initial read failed, force abort, you may see leak sanitizer warning and you can ignore "
                 "it\n");
      return nullptr;
    }
  }
  return txn_r;
}

auto StartWriteTxn(IsolationLevel write_txn_level, BustubInstance &instance, bool is_delete) -> Transaction * {
  auto writer = bustub::SimpleStreamWriter(std::cout, true);
  fmt::print(stderr, "2: write txn: {}\n", write_txn_level);
  auto txn_w = instance.txn_manager_->Begin(nullptr, write_txn_level);
  if (is_delete) {
    fmt::print(stderr, "2.1: delete v1 = 2\n");
    instance.ExecuteSqlTxn("DELETE FROM t1 WHERE v1 = 2", writer, txn_w);
  } else {
    fmt::print(stderr, "2.1: insert new values\n");
    instance.ExecuteSqlTxn("INSERT INTO t1 VALUES (233, 1), (233, 2), (233, 3)", writer, txn_w);
  }
  fmt::print(stderr, "2.2: X locks\n");
  for (const auto &[t, v] : *txn_w->GetExclusiveRowLockSet()) {
    for (const auto &l : v) {
      std::cout << "table: " << t << " " << l;
    }
  }
  return txn_w;
}

auto TryRead(Transaction *txn_r, BustubInstance &instance, ExpectedOutcome o, bool is_delete,
             std::atomic<bool> &check_done) -> bool {
  bool ret = false;
  std::stringstream result;
  auto writer = bustub::SimpleStreamWriter(result, true, ",");
  fmt::print(stderr, "3: read txn read\n");
  fmt::print(stderr, "3.1: read values\n");
  instance.ExecuteSqlTxn("SELECT * FROM t1;", writer, txn_r);
  fmt::print(stderr, "3.2: R locks\n");
  for (const auto &[t, v] : *txn_r->GetSharedRowLockSet()) {
    for (const auto &l : v) {
      std::cout << "table: " << t << " " << l;
    }
  }
  fmt::print(stderr, "3.3: read done\n");
  switch (o) {
    case ExpectedOutcome::DirtyRead:
      if (is_delete) {
        ret = ExpectResult(result.str(), "1,2,\n1,3,\n1,4,");
      } else {
        ret = ExpectResult(result.str(), "1,2,\n1,3,\n1,4,\n2,2,\n2,3,\n2,4,\n233,1,\n233,2,\n233,3,\n");
      }
      break;
    case ExpectedOutcome::Read:
      ret = ExpectResult(result.str(), "1,2,\n1,3,\n1,4,\n2,2,\n2,3,\n2,4,");
      break;
    case ExpectedOutcome::BlockOnRead:
      if (!check_done.load(std::memory_order_seq_cst)) {
        fmt::print(stderr, "ERROR: read txn should BlockOnRead, it reads instead\n");
        fmt::print(stderr, "{}", result.str());
        ret = false;
      } else {
        ret = true;
      }
      break;
    default:
      UNREACHABLE("unexpected outcome");
  }
  return ret;
}

void ExpectTwoTxn(const std::string &test_name, IsolationLevel read_txn_level, IsolationLevel write_txn_level,
                  bool read_before_write, bool is_delete, ExpectedOutcome o) {
  fmt::print(stderr, "--- {} ---\n", test_name);
  auto instance = GetDbForIsolationTest();

  auto txn_r = StartReadTxn(read_txn_level, *instance, read_before_write);
  if (txn_r == nullptr) {
    return;
  }

  Transaction *txn_w = nullptr;
  if (o != ExpectedOutcome::BlockOnWrite) {
    txn_w = StartWriteTxn(write_txn_level, *instance, is_delete);
  }

  std::promise<bool> succeed;
  std::atomic<bool> check_done{false};
  auto future = succeed.get_future();

  std::thread thread_may_block(
      [instance, txn_r, o, &check_done, write_txn_level, is_delete](std::promise<bool> succeed) {
        if (o != ExpectedOutcome::BlockOnWrite) {
          succeed.set_value(TryRead(txn_r, *instance, o, is_delete, check_done));
          instance->txn_manager_->Commit(txn_r);
          delete txn_r;
        } else {
          auto txn_w = StartWriteTxn(write_txn_level, *instance, is_delete);
          succeed.set_value(true);
          instance->txn_manager_->Commit(txn_w);
          delete txn_w;
        }
      },
      std::move(succeed));

  fmt::print(stderr, "4: wait for result\n");
  switch (o) {
    case ExpectedOutcome::DirtyRead:
    case ExpectedOutcome::Read:
      thread_may_block.join();
      EXPECT_TRUE(future.get());
      instance->txn_manager_->Commit(txn_w);
      delete txn_w;
      break;
    case ExpectedOutcome::BlockOnRead:
      // TODO(student): if you are using a debugger, you probably want to uncomment this line.
      // future.wait();

      EXPECT_FALSE(future.wait_for(std::chrono::seconds(1)) == std::future_status::ready);
      instance->txn_manager_->Commit(txn_w);
      delete txn_w;
      check_done.store(true, std::memory_order_seq_cst);
      thread_may_block.join();
      break;
    case ExpectedOutcome::BlockOnWrite:
      // TODO(student): if you are using a debugger, you probably want to uncomment this line.
      // future.wait();

      EXPECT_FALSE(future.wait_for(std::chrono::seconds(1)) == std::future_status::ready);
      instance->txn_manager_->Commit(txn_r);
      delete txn_r;
      thread_may_block.join();
      break;
  }
  fmt::print(stderr, "5: test reaches the end\n\n\n");
}

const bool READ_BEFORE_WRITE = true;
const bool IS_INSERT = false;
const bool IS_DELETE = true;

}  // namespace bustub
