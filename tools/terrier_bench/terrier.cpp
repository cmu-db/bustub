#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>  // NOLINT
#include <random>
#include <sstream>
#include <string>
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
#include "terrier_bench_config.h"

#include <sys/time.h>

auto ClockMs() -> uint64_t {
  struct timeval tm;
  gettimeofday(&tm, nullptr);
  return static_cast<uint64_t>(tm.tv_sec * 1000) + static_cast<uint64_t>(tm.tv_usec / 1000);
}

static const size_t BUSTUB_NFT_NUM = 30000;
static const size_t BUSTUB_TERRIER_THREAD = 2;
static const size_t BUSTUB_TERRIER_CNT = 100;

struct TerrierTotalMetrics {
  uint64_t aborted_count_txn_cnt_{0};
  uint64_t committed_count_txn_cnt_{0};
  uint64_t aborted_update_txn_cnt_{0};
  uint64_t committed_update_txn_cnt_{0};
  uint64_t start_time_{0};
  std::mutex mutex_;

  void Begin() { start_time_ = ClockMs(); }

  void ReportCount(uint64_t aborted_cnt, uint64_t committed_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    aborted_count_txn_cnt_ += aborted_cnt;
    committed_count_txn_cnt_ += committed_cnt;
  }

  void ReportUpdate(uint64_t aborted_cnt, uint64_t committed_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    aborted_update_txn_cnt_ += aborted_cnt;
    committed_update_txn_cnt_ += committed_cnt;
  }

  void Report() {
    auto now = ClockMs();
    auto elsped = now - start_time_;
    auto count_txn_per_sec = committed_count_txn_cnt_ / static_cast<double>(elsped) * 1000;
    auto update_txn_per_sec = committed_update_txn_cnt_ / static_cast<double>(elsped) * 1000;

    fmt::print("<<< BEGIN\n");
    fmt::print("update: {}\n", update_txn_per_sec);
    fmt::print("count: {}\n", count_txn_per_sec);
    fmt::print(">>> END\n");
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
    auto elsped = now - start_time_;
    if (elsped - last_report_at_ > 1000) {
      fmt::print("{}: total_committed_txn={:<5} total_aborted_txn={:<5} throughput={:<6.3} avg_throughput={:<6.3}\n",
                 reporter_, committed_txn_cnt_, aborted_txn_cnt_,
                 (committed_txn_cnt_ - last_committed_txn_cnt_) / static_cast<double>(elsped - last_report_at_) * 1000,
                 committed_txn_cnt_ / static_cast<double>(elsped) * 1000);
      last_report_at_ = elsped;
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

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  argparse::ArgumentParser program("bustub-terrier-bench");
  program.add_argument("--duration").help("run terrier bench for n milliseconds");
  program.add_argument("--force-create-index").help("create index in terrier bench");
  program.add_argument("--force-enable-update").help("use update statement in terrier bench");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  auto bustub = std::make_unique<bustub::BustubInstance>();
  auto writer = bustub::SimpleStreamWriter(std::cerr);

  // create schema
  auto schema = "CREATE TABLE nft(id int, terrier int);";
  std::cerr << "x: create schema" << std::endl;
  bustub->ExecuteSql(schema, writer);

  // create index

#ifdef TERRIER_BENCH_ENABLE_INDEX
  bool enable_index = true;
#else
  bool enable_index = false;
#endif

  if (program.present("--force-create-index")) {
    enable_index = ParseBool(program.get("--force-create-index"));
  }

  if (enable_index) {
    auto schema = "CREATE INDEX nftid on nft(id);";
    std::cerr << "x: create index" << std::endl;
    bustub->ExecuteSql(schema, writer);
  } else {
    std::cerr << "x: create index disabled" << std::endl;
  }

#ifdef TERRIER_BENCH_ENABLE_UPDATE
  bool enable_update = true;
#else
  bool enable_update = false;
#endif
  if (program.present("--force-enable-update")) {
    enable_update = ParseBool(program.get("--force-enable-update"));
  }

  if (enable_update) {
    std::cerr << "x: use update statement" << std::endl;
  } else {
    std::cerr << "x: use insert + delete" << std::endl;
  }

  uint64_t duration_ms = 30000;

  if (program.present("--duration")) {
    duration_ms = std::stoi(program.get("--duration"));
  }

  std::cerr << "x: benchmark for " << duration_ms << "ms" << std::endl;

  // initialize data
  std::cerr << "x: initialize data" << std::endl;
  std::string query = "INSERT INTO nft VALUES ";
  for (size_t i = 0; i < BUSTUB_NFT_NUM; i++) {
    query += fmt::format("({}, {})", i, 0);
    if (i != BUSTUB_NFT_NUM - 1) {
      query += ", ";
    } else {
      query += ";";
    }
  }

  {
    std::stringstream ss;
    auto writer = bustub::SimpleStreamWriter(ss, true);
    auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
    bustub->ExecuteSqlTxn(query, writer, txn);
    bustub->txn_manager_->Commit(txn);
    delete txn;
    if (ss.str() != fmt::format("{}\t\n", BUSTUB_NFT_NUM)) {
      fmt::print("unexpected result \"{}\" when insert\n", ss.str());
      exit(1);
    }
  }

  std::cerr << "x: benchmark start" << std::endl;

  std::vector<std::thread> threads;
  TerrierTotalMetrics total_metrics;

  total_metrics.Begin();

  for (size_t thread_id = 0; thread_id < BUSTUB_TERRIER_THREAD; thread_id++) {
    threads.emplace_back(std::thread([thread_id, &bustub, enable_update, duration_ms, &total_metrics] {
      const size_t nft_range_size = BUSTUB_NFT_NUM / BUSTUB_TERRIER_THREAD;
      const size_t nft_range_begin = thread_id * nft_range_size;
      const size_t nft_range_end = (thread_id + 1) * nft_range_size;
      std::random_device r;
      std::default_random_engine gen(r());
      std::uniform_int_distribution<int> nft_uniform_dist(nft_range_begin, nft_range_end - 1);
      std::uniform_int_distribution<int> terrier_uniform_dist(0, BUSTUB_TERRIER_CNT - 1);

      TerrierMetrics metrics(fmt::format("Update {}", thread_id), duration_ms);
      metrics.Begin();

      while (!metrics.ShouldFinish()) {
        std::stringstream ss;
        auto writer = bustub::SimpleStreamWriter(ss, true);
        auto nft_id = nft_uniform_dist(gen);
        auto terrier_id = terrier_uniform_dist(gen);
        bool txn_success = true;

        if (enable_update) {
          auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
          std::string query = fmt::format("UPDATE nft SET terrier = {} WHERE id = {}", terrier_id, nft_id);
          if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
            txn_success = false;
          }

          if (txn_success && ss.str() != "1\t\n") {
            fmt::print("unexpected result \"{}\", you should report txn fail if it is aborted\n", ss.str());
            exit(1);
          }

          if (txn_success) {
            bustub->txn_manager_->Commit(txn);
            metrics.TxnCommitted();
          } else {
            bustub->txn_manager_->Abort(txn);
            metrics.TxnAborted();
          }
          delete txn;
        } else {
          auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);

          std::string query = fmt::format("DELETE FROM nft WHERE id = {}", nft_id);
          if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
            txn_success = false;
          }

          if (txn_success && ss.str() != "1\t\n") {
            fmt::print("unexpected result \"{}\", you should report txn fail if it is aborted\n", ss.str());
            exit(1);
          }

          if (!txn_success) {
            bustub->txn_manager_->Abort(txn);
            metrics.TxnAborted();
            delete txn;
          } else {
            bustub->txn_manager_->Commit(txn);
            delete txn;

            txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);

            query = fmt::format("INSERT INTO nft VALUES ({}, {})", nft_id, terrier_id);
            if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
              txn_success = false;
            }

            if (txn_success && ss.str() != "1\t\n1\t\n") {
              fmt::print("unexpected result \"{}\", you should report txn fail if it is aborted\n", ss.str());
              exit(1);
            }

            if (!txn_success) {
              bustub->txn_manager_->Abort(txn);
              metrics.TxnAborted();
            } else {
              bustub->txn_manager_->Commit(txn);
              metrics.TxnCommitted();
            }
            delete txn;
          }
        }

        metrics.Report();
      }

      total_metrics.ReportUpdate(metrics.aborted_txn_cnt_, metrics.committed_txn_cnt_);
    }));
  }

  for (size_t thread_id = 0; thread_id < BUSTUB_TERRIER_THREAD; thread_id++) {
    threads.emplace_back(std::thread([thread_id, &bustub, duration_ms, &total_metrics] {
      std::random_device r;
      std::default_random_engine gen(r());
      std::uniform_int_distribution<int> terrier_uniform_dist(0, BUSTUB_TERRIER_CNT - 1);

      TerrierMetrics metrics(fmt::format(" Count {}", thread_id), duration_ms);
      metrics.Begin();

      while (!metrics.ShouldFinish()) {
        std::stringstream ss;
        auto writer = bustub::SimpleStreamWriter(ss, true);
        auto terrier_id = terrier_uniform_dist(gen);

        auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
        bool txn_success = true;

        std::string query = fmt::format("SELECT count(*) FROM nft WHERE terrier = {}", terrier_id);
        if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
          txn_success = false;
        }

        if (txn_success) {
          bustub->txn_manager_->Commit(txn);
          metrics.TxnCommitted();
        } else {
          bustub->txn_manager_->Abort(txn);
          metrics.TxnAborted();
        }
        delete txn;

        metrics.Report();
      }

      total_metrics.ReportCount(metrics.aborted_txn_cnt_, metrics.committed_txn_cnt_);
    }));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  {
    std::stringstream ss;
    auto writer = bustub::SimpleStreamWriter(ss, true);
    auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
    bustub->ExecuteSqlTxn("SELECT count(*) FROM nft", writer, txn);
    bustub->txn_manager_->Commit(txn);
    delete txn;
    if (ss.str() != fmt::format("{}\t\n", BUSTUB_NFT_NUM)) {
      fmt::print("unexpected result \"{}\" when verifying\n", ss.str());
      exit(1);
    }
  }

  total_metrics.Report();

  return 0;
}
