#include <algorithm>
#include <atomic>
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>  // NOLINT
#include <random>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "argparse/argparse.hpp"
#include "binder/binder.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "concurrency/transaction.h"
#include "concurrency/transaction_manager.h"
#include "fmt/core.h"
#include "fmt/std.h"

#include <sys/time.h>

auto ClockMs() -> uint64_t {
  struct timeval tm;
  gettimeofday(&tm, nullptr);
  return static_cast<uint64_t>(tm.tv_sec * 1000) + static_cast<uint64_t>(tm.tv_usec / 1000);
}

struct TerrierTotalMetrics {
  uint64_t aborted_transfer_txn_cnt_{0};
  uint64_t committed_transfer_txn_cnt_{0};
  uint64_t aborted_join_txn_cnt_{0};
  uint64_t committed_join_txn_cnt_{0};
  uint64_t start_time_{0};
  uint64_t elapsed_{0};
  std::mutex mutex_;

  void Begin() { start_time_ = ClockMs(); }

  void ReportTransfer(uint64_t aborted_cnt, uint64_t committed_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    aborted_transfer_txn_cnt_ += aborted_cnt;
    committed_transfer_txn_cnt_ += committed_cnt;
  }

  void ReportJoin(uint64_t aborted_cnt, uint64_t committed_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    aborted_join_txn_cnt_ += aborted_cnt;
    committed_join_txn_cnt_ += committed_cnt;
  }

  void End() {
    auto now = ClockMs();
    elapsed_ = now - start_time_;
  }

  void Report(uint64_t db_size) {
    auto transfer_txn_per_sec = committed_transfer_txn_cnt_ / static_cast<double>(elapsed_) * 1000;
    auto join_txn_per_sec = committed_join_txn_cnt_ / static_cast<double>(elapsed_) * 1000;

    fmt::print(stderr, "<<< BEGIN\n");

    fmt::print(stderr, "transfer: {}\n", transfer_txn_per_sec);
    fmt::print(stderr, "join: {}\n", join_txn_per_sec);
    fmt::print(stderr, "db_size: {}\n", db_size);

    fmt::print(stderr, ">>> END\n");
  }
};

struct TerrierMetrics {
  uint64_t start_time_{0};
  uint64_t last_report_at_{0};
  uint64_t last_committed_txn_cnt_{0};
  uint64_t last_aborted_txn_cnt_{0};
  uint64_t committed_txn_cnt_{0};
  uint64_t aborted_txn_cnt_{0};
  std::string reporter_;
  uint64_t duration_ms_;

  explicit TerrierMetrics(std::string reporter, uint64_t duration_ms)
      : reporter_(std::move(reporter)), duration_ms_(duration_ms) {}

  void TxnAborted() { aborted_txn_cnt_ += 1; }

  void TxnCommitted() { committed_txn_cnt_ += 1; }

  void Begin() { start_time_ = ClockMs(); }

  void Report() {
    auto now = ClockMs();
    auto elapsed = now - start_time_;
    if (elapsed - last_report_at_ > 1000) {
      fmt::print(stderr,
                 "[{:5.2f}] {}: total_committed_txn={:<8} total_aborted_txn={:<8} throughput={:<8.3f} "
                 "avg_throughput={:<8.3f}\n",
                 elapsed / 1000.0, reporter_, committed_txn_cnt_, aborted_txn_cnt_,
                 (committed_txn_cnt_ - last_committed_txn_cnt_) / static_cast<double>(elapsed - last_report_at_) * 1000,
                 committed_txn_cnt_ / static_cast<double>(elapsed) * 1000);
      last_report_at_ = elapsed;
      last_committed_txn_cnt_ = committed_txn_cnt_;
    }
  }

  auto ShouldFinish() -> bool {
    auto now = ClockMs();
    return now - start_time_ > duration_ms_;
  }
};

auto ParseBool(const std::string &str) -> bool {
  if (str == "no" || str == "false") {
    return false;
  }
  if (str == "yes" || str == "true") {
    return true;
  }
  throw bustub::Exception(fmt::format("unexpected arg: {}", str));
}

auto ExtractOneCell(const bustub::StringVectorWriter &writer) -> std::string {
  if (writer.values_.size() == 1) {
    if (writer.values_[0].size() == 1) {
      return writer.values_[0][0];
    }
  }
  fmt::println(stderr, "expect only one cell in the result set");
  std::terminate();
}

void Bench1TaskTransfer(const int thread_id, const int terrier_num, const uint64_t duration_ms,
                        const bustub::IsolationLevel iso_lvl, bustub::BustubInstance *bustub,
                        TerrierTotalMetrics &total_metrics) {
  const int max_transfer_amount = 1000;
  std::random_device r;
  std::default_random_engine gen(r());
  std::uniform_int_distribution<int> terrier_uniform_dist(0, terrier_num - 1);
  std::uniform_int_distribution<int> money_transfer_dist(5, max_transfer_amount);

  TerrierMetrics metrics(fmt::format("Xfr  {}", thread_id), duration_ms);
  metrics.Begin();

  while (!metrics.ShouldFinish()) {
    std::stringstream ss;
    auto writer = bustub::StringVectorWriter();
    int terrier_a;
    int terrier_b;
    do {
      terrier_a = terrier_uniform_dist(gen);
      terrier_b = terrier_uniform_dist(gen);
    } while (terrier_a == terrier_b);
    int transfer_amount = money_transfer_dist(gen);
    auto txn = bustub->txn_manager_->Begin(iso_lvl);
    std::string query1 =
        fmt::format("UPDATE terriers SET token = token + {} WHERE terrier = {}", transfer_amount, terrier_a);
    std::string query2 =
        fmt::format("UPDATE terriers SET token = token - {} WHERE terrier = {}", transfer_amount, terrier_b);
    if (!bustub->ExecuteSqlTxn(query1, writer, txn)) {
      bustub->txn_manager_->Abort(txn);
      metrics.TxnAborted();
      continue;
    }
    auto result = ExtractOneCell(writer);
    if (result != "1") {
      fmt::print(stderr, "unexpected result when update \"{}\" != 1\n", result);
      exit(1);
    }
    if (!bustub->ExecuteSqlTxn(query2, writer, txn)) {
      bustub->txn_manager_->Abort(txn);
      metrics.TxnAborted();
      continue;
    }
    result = ExtractOneCell(writer);
    if (result != "1") {
      fmt::print(stderr, "unexpected result when update \"{}\" != 1\n", result);
      exit(1);
    }
    if (!bustub->txn_manager_->Commit(txn)) {
      metrics.TxnAborted();
      continue;
    }
    metrics.TxnCommitted();
    metrics.Report();
  }

  total_metrics.ReportTransfer(metrics.aborted_txn_cnt_, metrics.committed_txn_cnt_);
}

void Bench2TaskTransfer(const int thread_id, const int terrier_num, const uint64_t duration_ms,
                        const bustub::IsolationLevel iso_lvl, bustub::BustubInstance *bustub,
                        TerrierTotalMetrics &total_metrics, std::atomic<int> &token_adjustment) {
  const int max_transfer_amount = 1000;
  std::random_device r;
  std::default_random_engine gen(r());
  std::uniform_int_distribution<int> terrier_uniform_dist(0, terrier_num - 1);
  std::uniform_int_distribution<int> money_transfer_dist(5, max_transfer_amount);
  int adjustment = 0;

  TerrierMetrics metrics(fmt::format("Xfr  {}", thread_id), duration_ms);
  metrics.Begin();

  while (!metrics.ShouldFinish()) {
    std::stringstream ss;
    auto writer = bustub::StringVectorWriter();
    int terrier_a;
    int terrier_b;
    do {
      terrier_a = terrier_uniform_dist(gen);
      terrier_b = terrier_uniform_dist(gen);
    } while (terrier_a == terrier_b);
    int transfer_amount = money_transfer_dist(gen);
    auto txn = bustub->txn_manager_->Begin(iso_lvl);
    std::string select1 = fmt::format("SELECT network FROM terriers WHERE terrier = {}", terrier_a);
    std::string select2 = fmt::format("SELECT network FROM terriers WHERE terrier = {}", terrier_b);
    if (!bustub->ExecuteSqlTxn(select1, writer, txn)) {
      bustub->txn_manager_->Abort(txn);
      metrics.TxnAborted();
      continue;
    }
    auto network1 = ExtractOneCell(writer);
    if (!bustub->ExecuteSqlTxn(select2, writer, txn)) {
      bustub->txn_manager_->Abort(txn);
      metrics.TxnAborted();
      continue;
    }
    auto network2 = ExtractOneCell(writer);
    int receive = network1 == network2 ? transfer_amount : transfer_amount * 0.97;

    std::string transfer1 =
        fmt::format("UPDATE terriers SET token = token + {} WHERE terrier = {}", receive, terrier_a);
    std::string transfer2 =
        fmt::format("UPDATE terriers SET token = token - {} WHERE terrier = {}", transfer_amount, terrier_b);

    if (!bustub->ExecuteSqlTxn(transfer1, writer, txn)) {
      bustub->txn_manager_->Abort(txn);
      metrics.TxnAborted();
      continue;
    }
    auto result = ExtractOneCell(writer);
    if (result != "1") {
      fmt::print(stderr, "unexpected result when update \"{}\" != 1\n", result);
      exit(1);
    }
    if (!bustub->ExecuteSqlTxn(transfer2, writer, txn)) {
      bustub->txn_manager_->Abort(txn);
      metrics.TxnAborted();
      continue;
    }
    result = ExtractOneCell(writer);
    if (result != "1") {
      fmt::print(stderr, "unexpected result when update \"{}\" != 1\n", result);
      exit(1);
    }
    metrics.TxnCommitted();

    if (!bustub->txn_manager_->Commit(txn)) {
      metrics.TxnAborted();
      continue;
    }

    adjustment += receive - transfer_amount;  // losing some tokens...

    metrics.TxnCommitted();
    metrics.Report();
  }
  token_adjustment.fetch_add(adjustment);
  total_metrics.ReportTransfer(metrics.aborted_txn_cnt_, metrics.committed_txn_cnt_);
}

void Bench2TaskJoin(const int thread_id, const int terrier_num, const uint64_t duration_ms,
                    const bustub::IsolationLevel iso_lvl, bustub::BustubInstance *bustub,
                    TerrierTotalMetrics &total_metrics, std::atomic<int> &token_adjustment) {
  std::random_device r;
  std::default_random_engine gen(r());
  std::uniform_int_distribution<int> terrier_uniform_dist(0, terrier_num - 1);
  std::uniform_int_distribution<int> join_target_dist(0, terrier_num - 1);
  std::uniform_int_distribution<int> join_action_dist(0, 2);

  TerrierMetrics metrics(fmt::format("Join {}", thread_id), duration_ms);
  metrics.Begin();

  const int sign_bonus = 1000;
  int adjustment = 0;

  while (!metrics.ShouldFinish()) {
    std::stringstream ss;
    auto writer = bustub::StringVectorWriter();
    int join_action = join_action_dist(gen);
    int terrier;
    int join_target;
    do {
      terrier = terrier_uniform_dist(gen);
      join_target = join_target_dist(gen);
    } while (terrier == join_target);

    if (join_action == 0) {
      // join another terrier's network
      auto txn = bustub->txn_manager_->Begin(iso_lvl);
      std::string target_network_sql = fmt::format("SELECT network FROM terriers WHERE terrier = {}", join_target);
      if (!bustub->ExecuteSqlTxn(target_network_sql, writer, txn)) {
        bustub->txn_manager_->Abort(txn);
        metrics.TxnAborted();
        continue;
      }
      auto target_network = ExtractOneCell(writer);
      std::string join_sql = fmt::format("UPDATE terriers SET network = {}, token = token + {} WHERE terrier = {}",
                                         target_network, sign_bonus, terrier);
      if (!bustub->ExecuteSqlTxn(join_sql, writer, txn)) {
        bustub->txn_manager_->Abort(txn);
        metrics.TxnAborted();
        continue;
      }
      auto result = ExtractOneCell(writer);
      if (result != "1") {
        fmt::print(stderr, "unexpected result when update \"{}\" != 1\n", result);
        exit(1);
      }
      if (!bustub->txn_manager_->Commit(txn)) {
        metrics.TxnAborted();
        continue;
      }
      adjustment += sign_bonus;  // adjust up
      metrics.TxnCommitted();
      metrics.Report();
    } else if (join_action == 1) {
      // create a new network
      auto txn = bustub->txn_manager_->Begin(iso_lvl);
      std::string join_sql = fmt::format("UPDATE terriers SET network = {}, token = token - {} WHERE terrier = {}",
                                         join_target, sign_bonus, terrier);
      if (!bustub->ExecuteSqlTxn(join_sql, writer, txn)) {
        bustub->txn_manager_->Abort(txn);
        metrics.TxnAborted();
        continue;
      }
      auto result = ExtractOneCell(writer);
      if (result != "1") {
        fmt::print(stderr, "unexpected result when update \"{}\" != 1\n", result);
        exit(1);
      }
      if (!bustub->txn_manager_->Commit(txn)) {
        metrics.TxnAborted();
        continue;
      }
      adjustment -= sign_bonus;  // adjust down
      metrics.TxnCommitted();
      metrics.Report();
    }
    if (join_action == 2) {
      // trigger serializable verification by two txns join each other, one should abort
      auto txn1 = bustub->txn_manager_->Begin(iso_lvl);
      auto txn2 = bustub->txn_manager_->Begin(iso_lvl);
      auto terrier1 = terrier;
      auto terrier2 = join_target;
      std::string network1_sql = fmt::format("SELECT network FROM terriers WHERE terrier = {}", terrier2);
      std::string network2_sql = fmt::format("SELECT network FROM terriers WHERE terrier = {}", terrier1);
      if (!bustub->ExecuteSqlTxn(network1_sql, writer, txn1)) {
        bustub->txn_manager_->Abort(txn1);
        bustub->txn_manager_->Abort(txn2);
        metrics.TxnAborted();
        continue;
      }
      auto network2 = ExtractOneCell(writer);
      if (!bustub->ExecuteSqlTxn(network2_sql, writer, txn2)) {
        bustub->txn_manager_->Abort(txn1);
        bustub->txn_manager_->Abort(txn2);
        metrics.TxnAborted();
        continue;
      }
      auto network1 = ExtractOneCell(writer);

      std::string join1_sql = fmt::format("UPDATE terriers SET network = {}, token = token + {} WHERE terrier = {}",
                                          network2, sign_bonus, terrier1);
      std::string join2_sql = fmt::format("UPDATE terriers SET network = {}, token = token + {} WHERE terrier = {}",
                                          network1, sign_bonus, terrier2);
      if (!bustub->ExecuteSqlTxn(join1_sql, writer, txn1)) {
        bustub->txn_manager_->Abort(txn1);
        bustub->txn_manager_->Abort(txn2);
        metrics.TxnAborted();
        continue;
      }
      auto result = ExtractOneCell(writer);
      if (result != "1") {
        fmt::print(stderr, "unexpected result when update \"{}\" != 1\n", result);
        exit(1);
      }
      if (!bustub->ExecuteSqlTxn(join2_sql, writer, txn2)) {
        bustub->txn_manager_->Abort(txn1);
        bustub->txn_manager_->Abort(txn2);
        metrics.TxnAborted();
        continue;
      }
      result = ExtractOneCell(writer);
      if (result != "1") {
        fmt::print(stderr, "unexpected result when update \"{}\" != 1\n", result);
        exit(1);
      }
      if (!bustub->txn_manager_->Commit(txn1)) {
        bustub->txn_manager_->Abort(txn2);
        metrics.TxnAborted();
        continue;
      }
      adjustment += sign_bonus;  // only one of them succeeded
      if (!bustub->txn_manager_->Commit(txn2)) {
        metrics.TxnAborted();
        continue;
      }
      fmt::println(stderr, "one of the txns should be aborted!");
      std::terminate();
    }
  }

  token_adjustment.fetch_add(adjustment);
  total_metrics.ReportJoin(metrics.aborted_txn_cnt_, metrics.committed_txn_cnt_);
}

auto ComputeDbSize(bustub::BustubInstance *bustub, const std::string &table_name) {
  auto table_info = bustub->catalog_->GetTable(table_name);
  auto iter = table_info->table_->MakeEagerIterator();
  int cnt = 0;
  while (!iter.IsEnd()) {
    ++cnt;
    ++iter;
  }
  int undo_cnt = 0;
  std::shared_lock<std::shared_mutex> lck(bustub->txn_manager_->txn_map_mutex_);
  const auto map = bustub->txn_manager_->txn_map_;
  lck.unlock();
  for (const auto &[_, txn] : map) {
    cnt += txn->GetUndoLogNum();
  }
  return cnt + undo_cnt;
}

void TaskComputeDbSize(const uint64_t duration_ms, std::atomic<int> &db_size, bustub::BustubInstance *bustub,
                       const std::string &table_name) {
  TerrierMetrics metrics("Compute Size", duration_ms);
  metrics.Begin();
  while (!metrics.ShouldFinish()) {
    auto size = ComputeDbSize(bustub, table_name);
    if (size > db_size.load()) {
      db_size.store(size);
    }
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }
}

void PrintPlan(bustub::BustubInstance &instance, const std::string &query, bool ensure_index_scan = true) {
  {
    std::stringstream ss;
    bustub::SimpleStreamWriter writer_ss(ss);
    auto *txn = instance.txn_manager_->Begin();
    instance.ExecuteSqlTxn("EXPLAIN (o) " + query, writer_ss, txn);
    fmt::println(stderr, "> {}\n[PLAN]\n{}", query, ss.str());
    if (ensure_index_scan) {
      if (!bustub::StringUtil::Contains(ss.str(), "IndexScan")) {
        fmt::println(stderr, "ERROR: index scan not found in plan.");
        std::terminate();
      }
    }
  }
}

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  const auto isolation_lvl = bustub::IsolationLevel::SNAPSHOT_ISOLATION;
  argparse::ArgumentParser program("bustub-terrier-bench");
  program.add_argument("--duration").help("run terrier bench for n milliseconds");
  program.add_argument("--terriers").help("number of terriers in the bench");
  program.add_argument("--threads").help("number of threads in the bench");
  program.add_argument("--commit-threshold").help("number of commits to pass the benchmark");
  program.add_argument("--bench-1")
      .default_value(false)
      .implicit_value(true)
      .help("run benchmark #1 (in snapshot isolation)");
  program.add_argument("--bench-2")
      .default_value(false)
      .implicit_value(true)
      .help("run benchmark #2 (in serializable)");

  size_t bustub_terrier_num = 10;
  size_t bustub_thread_cnt = 2;
  const size_t bpm_size = 4096;  // ensure benchmark does not hit BPM
  size_t commit_threshold = 100;

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl << program << std::endl;
    return 1;
  }

  auto bustub = std::make_unique<bustub::BustubInstance>(bpm_size);
  auto writer = bustub::SimpleStreamWriter(std::cerr);

  if (program.present("--terriers")) {
    bustub_terrier_num = std::stoi(program.get("--terriers"));
  }

  if (program.present("--commit-threshold")) {
    commit_threshold = std::stoi(program.get("--commit-threshold"));
  }

  if (program.present("--threads")) {
    bustub_thread_cnt = std::stoi(program.get("--threads"));
  }

  uint64_t duration_ms = 30000;

  if (program.present("--duration")) {
    duration_ms = std::stoi(program.get("--duration"));
  }

  fmt::println(stderr, "x: benchmark for {} ms", duration_ms);
  fmt::println(stderr, "x: terrier_num={}", bustub_terrier_num);
  fmt::println(stderr, "x: thread_cnt={}", bustub_thread_cnt);

  bool bench_1 = program.get<bool>("--bench-1");
  bool bench_2 = program.get<bool>("--bench-2");
  if ((bench_1 && bench_2) || (!bench_1 && !bench_2)) {
    fmt::println(stderr, "only one of the bench should be selected");
    std::terminate();
  }

  if (bench_1) {
    auto schema = "CREATE TABLE terriers(terrier int PRIMARY KEY, token int);";
    fmt::println(stderr, "x: create schema for benchmark #1");
    bustub->ExecuteSql(schema, writer);
    fmt::println(stderr, "x: please ensure plans are correct for all queries");
    PrintPlan(*bustub, "UPDATE terriers SET token = token + 1 WHERE terrier = 0");
    PrintPlan(*bustub, "UPDATE terriers SET token = token - 1 WHERE terrier = 0");
    PrintPlan(*bustub, "DELETE FROM terriers WHERE terrier = 0");
    PrintPlan(*bustub, "INSERT INTO terriers VALUES (0, 0)", false);
    PrintPlan(*bustub, "SELECT * from terriers", false);
  }
  if (bench_2) {
    auto schema = "CREATE TABLE terriers(terrier int PRIMARY KEY, token int, network int);";
    fmt::println(stderr, "x: create schema for benchmark #2");
    bustub->ExecuteSql(schema, writer);
    fmt::println(stderr, "x: please ensure plans are correct for all queries");
    PrintPlan(*bustub, "UPDATE terriers SET token = token + 1 WHERE terrier = 0");
    PrintPlan(*bustub, "UPDATE terriers SET token = token - 1 WHERE terrier = 0");
    PrintPlan(*bustub, "UPDATE terriers SET network = 1, token = token + 1000 WHERE terrier = 0");
    PrintPlan(*bustub, "UPDATE terriers SET network = 1, token = token - 1000 WHERE terrier = 0");
    PrintPlan(*bustub, "SELECT network from terriers WHERE terrier = 0");
    PrintPlan(*bustub, "DELETE FROM terriers WHERE terrier = 0");
    PrintPlan(*bustub, "INSERT INTO terriers VALUES (0, 0, 0)", false);
    PrintPlan(*bustub, "SELECT * from terriers", false);
  }

  const int initial_token = 10000;
  std::atomic<int> token_adjustment{0};

  // initialize data
  fmt::println(stderr, "x: initialize data");
  std::string query = "INSERT INTO terriers VALUES ";
  for (size_t i = 0; i < bustub_terrier_num; i++) {
    if (bench_1) {
      query += fmt::format("({}, {})", i, initial_token);
      if (i != bustub_terrier_num - 1) {
        query += ", ";
      } else {
        query += ";";
      }
    }
    if (bench_2) {
      query += fmt::format("({}, {}, {})", i, initial_token, i);
      if (i != bustub_terrier_num - 1) {
        query += ", ";
      } else {
        query += ";";
      }
    }
  }

  {
    auto writer = bustub::StringVectorWriter();
    auto txn = bustub->txn_manager_->Begin(isolation_lvl);
    auto success = bustub->ExecuteSqlTxn(query, writer, txn);
    BUSTUB_ENSURE(success, "txn not success");
    BUSTUB_ENSURE(bustub->txn_manager_->Commit(txn), "cannot commit");
    auto result = ExtractOneCell(writer);
    if (result != std::to_string(bustub_terrier_num)) {
      fmt::print(stderr, "unexpected result \"{}\" when insert\n", result);
      std::terminate();
    }
  }

  bustub::global_disable_execution_exception_print.store(true);

  fmt::println(stderr, "x: benchmark start");

  std::vector<std::thread> threads;
  TerrierTotalMetrics total_metrics;

  total_metrics.Begin();

  for (size_t thread_id = 0; thread_id < bustub_thread_cnt; thread_id++) {
    if (bench_1) {
      threads.emplace_back([thread_id, bustub_terrier_num, duration_ms, &bustub, &total_metrics]() {
        Bench1TaskTransfer(thread_id, bustub_terrier_num, duration_ms, bustub::IsolationLevel::SNAPSHOT_ISOLATION,
                           bustub.get(), total_metrics);
      });
    }
    if (bench_2) {
      if (thread_id % 2 == 0) {
        threads.emplace_back(
            [thread_id, bustub_terrier_num, duration_ms, &bustub, &total_metrics, &token_adjustment]() {
              Bench2TaskJoin(thread_id, bustub_terrier_num, duration_ms, bustub::IsolationLevel::SERIALIZABLE,
                             bustub.get(), total_metrics, token_adjustment);
            });
      } else {
        threads.emplace_back(
            [thread_id, bustub_terrier_num, duration_ms, &bustub, &total_metrics, &token_adjustment]() {
              Bench2TaskTransfer(thread_id, bustub_terrier_num, duration_ms, bustub::IsolationLevel::SERIALIZABLE,
                                 bustub.get(), total_metrics, token_adjustment);
            });
      }
    }
  }

  std::atomic<int> db_size(0);

  threads.emplace_back(
      [duration_ms, &db_size, &bustub]() { TaskComputeDbSize(duration_ms, db_size, bustub.get(), "terriers"); });

  for (auto &thread : threads) {
    thread.join();
  }

  total_metrics.End();

  {
    bustub::StringVectorWriter writer;
    bustub->ExecuteSql("SELECT SUM(token) FROM terriers", writer);
    auto cnt = std::stoi(ExtractOneCell(writer));
    int expected = bustub_terrier_num * initial_token + token_adjustment;
    if (cnt != expected) {
      fmt::println(stderr, "inconsistent total tokens: expected {}, found {}", expected, cnt);
      exit(1);
    }
  }

  {
    bustub::SimpleStreamWriter writer(std::cerr);
    fmt::println(stderr, "--- the following data might be manually inspected by TAs ---");
    bustub->ExecuteSql("SELECT * FROM terriers LIMIT 100", writer);
  }

  if (db_size.load() == 0) {
    db_size = 999999999;
  }

  total_metrics.Report(db_size);

  if (total_metrics.committed_transfer_txn_cnt_ <= commit_threshold) {
    fmt::println(stderr, "too many txn are aborted: {} txn committed, expect more than {}",
                 total_metrics.committed_transfer_txn_cnt_, commit_threshold);
    exit(1);
  }

  if (bench_2 && total_metrics.committed_join_txn_cnt_ <= commit_threshold) {
    fmt::println(stderr, "too many txn are aborted: {} txn committed, expect more than {}",
                 total_metrics.committed_join_txn_cnt_, commit_threshold);
    exit(1);
  }

  return 0;
}
