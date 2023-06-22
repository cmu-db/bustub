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

static const size_t BUSTUB_TERRIER_THREAD = 2;
static const size_t BUSTUB_TERRIER_CNT = 100;

struct TerrierTotalMetrics {
  uint64_t aborted_count_txn_cnt_{0};
  uint64_t committed_count_txn_cnt_{0};
  uint64_t aborted_update_txn_cnt_{0};
  uint64_t committed_update_txn_cnt_{0};
  uint64_t aborted_verify_txn_cnt_{0};
  uint64_t committed_verify_txn_cnt_{0};
  uint64_t start_time_{0};
  std::mutex mutex_;

  void Begin() { start_time_ = ClockMs(); }

  void ReportVerify(uint64_t aborted_cnt, uint64_t committed_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    aborted_verify_txn_cnt_ += aborted_cnt;
    committed_verify_txn_cnt_ += committed_cnt;
  }

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
    auto verify_txn_per_sec = committed_verify_txn_cnt_ / static_cast<double>(elsped) * 1000;

    fmt::print("<<< BEGIN\n");

    // ensure the verifying thread is not blocked
    fmt::print("update: {}\n", update_txn_per_sec);
    fmt::print("count: {}\n", count_txn_per_sec);
    fmt::print("verify: {}\n", verify_txn_per_sec);

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
      fmt::print(
          "[{:5.2f}] {}: total_committed_txn={:<5} total_aborted_txn={:<5} throughput={:<6.3} avg_throughput={:<6.3}\n",
          elsped / 1000.0, reporter_, committed_txn_cnt_, aborted_txn_cnt_,
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

void CheckTableLock(bustub::Transaction *txn) {
  if (!txn->GetExclusiveTableLockSet()->empty() || !txn->GetSharedTableLockSet()->empty()) {
    fmt::print("should not acquire S/X table lock, grab IS/IX instead");
    exit(1);
  }
}

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  argparse::ArgumentParser program("bustub-terrier-bench");
  program.add_argument("--duration").help("run terrier bench for n milliseconds");
  program.add_argument("--force-create-index").help("create index in terrier bench");
  program.add_argument("--force-enable-update").help("use update statement in terrier bench");
  program.add_argument("--nft").help("number of NFTs in the bench");

  size_t bustub_nft_num = 10;

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

  if (program.present("--nft")) {
    bustub_nft_num = std::stoi(program.get("--nft"));
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
  std::cerr << "x: nft_num=" << bustub_nft_num << std::endl;

  // initialize data
  std::cerr << "x: initialize data" << std::endl;
  std::string query = "INSERT INTO nft VALUES ";
  for (size_t i = 0; i < bustub_nft_num; i++) {
    query += fmt::format("({}, {})", i, 0);
    if (i != bustub_nft_num - 1) {
      query += ", ";
    } else {
      query += ";";
    }
  }

  {
    std::stringstream ss;
    auto writer = bustub::SimpleStreamWriter(ss, true);
    auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
    auto success = bustub->ExecuteSqlTxn(query, writer, txn);
    BUSTUB_ENSURE(success, "txn not success");
    bustub->txn_manager_->Commit(txn);
    delete txn;
    if (ss.str() != fmt::format("{}\t\n", bustub_nft_num)) {
      fmt::print("unexpected result \"{}\" when insert\n", ss.str());
      exit(1);
    }
  }

  std::cerr << "x: benchmark start" << std::endl;

  std::vector<std::thread> threads;
  TerrierTotalMetrics total_metrics;

  bool verbose = false;

  total_metrics.Begin();

  for (size_t thread_id = 0; thread_id < BUSTUB_TERRIER_THREAD; thread_id++) {
    threads.emplace_back(
        std::thread([verbose, thread_id, &bustub, enable_update, duration_ms, &total_metrics, bustub_nft_num] {
          const size_t nft_range_size = bustub_nft_num / BUSTUB_TERRIER_THREAD;
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

            if (verbose) {
              fmt::print("begin: thread {} update nft {} to terrier {}\n", thread_id, nft_id, terrier_id);
            }

            if (enable_update) {
              auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
              std::string query = fmt::format("UPDATE nft SET terrier = {} WHERE id = {}", terrier_id, nft_id);
              if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
                txn_success = false;
              }

              if (txn_success && ss.str() != "1\t\n") {
                fmt::print("unexpected result \"{}\",\n", ss.str());
                exit(1);
              }

              if (txn_success) {
                CheckTableLock(txn);
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
                fmt::print("unexpected result \"{}\",\n", ss.str());
                exit(1);
              }

              if (!txn_success) {
                bustub->txn_manager_->Abort(txn);
                metrics.TxnAborted();
                delete txn;
              } else {
                query = fmt::format("INSERT INTO nft VALUES ({}, {})", nft_id, terrier_id);
                if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
                  txn_success = false;
                }

                if (txn_success && ss.str() != "1\t\n1\t\n") {
                  fmt::print("unexpected result \"{}\",\n", ss.str());
                  exit(1);
                }

                if (!txn_success) {
                  bustub->txn_manager_->Abort(txn);
                  metrics.TxnAborted();
                } else {
                  CheckTableLock(txn);
                  bustub->txn_manager_->Commit(txn);
                  metrics.TxnCommitted();
                }
                delete txn;
              }
            }

            if (verbose) {
              fmt::print("end  : thread {} update nft {} to terrier {}\n", thread_id, nft_id, terrier_id);
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
          CheckTableLock(txn);
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

  threads.emplace_back(std::thread([&bustub, duration_ms, &total_metrics, bustub_nft_num] {
    std::random_device r;
    std::default_random_engine gen(r());
    std::uniform_int_distribution<int> terrier_uniform_dist(0, BUSTUB_TERRIER_CNT - 1);

    TerrierMetrics metrics("  Verify", duration_ms);
    metrics.Begin();

    while (!metrics.ShouldFinish()) {
      std::stringstream ss;
      auto writer = bustub::SimpleStreamWriter(ss, true);

      auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
      bool txn_success = true;

      std::string query = "SELECT * FROM nft";
      if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
        txn_success = false;
      }

      if (txn_success) {
        auto all_nfts = bustub::StringUtil::Split(ss.str(), '\n');
        auto all_nfts_integer = std::vector<int>();
        for (auto &nft : all_nfts) {
          if (nft.empty()) {
            continue;
          }
          all_nfts_integer.push_back(std::stoi(nft));
        }
        std::sort(all_nfts_integer.begin(), all_nfts_integer.end());
        // Due to how BusTub works for now, it is impossible to get more than bustub_nft_num rows, but it is possible to
        // get fewer than that number.
        if (all_nfts_integer.size() != bustub_nft_num) {
          fmt::print("unexpected result when verifying length. scan result: {}, total rows: {}.\n",
                     all_nfts_integer.size(), bustub_nft_num);
          if (bustub_nft_num <= 100) {
            fmt::print("This is everything in your database:\n{}", ss.str());
          }
          exit(1);
        }
        for (int i = 0; i < static_cast<int>(bustub_nft_num); i++) {
          if (all_nfts_integer[i] != i) {
            fmt::print("unexpected result when verifying \"{} == {}\",\n", i, all_nfts_integer[i]);
            if (bustub_nft_num <= 100) {
              fmt::print("This is everything in your database:\n{}", ss.str());
            }
            exit(1);
          }
        }

        // query again, check repeatable read
        std::string query = "SELECT * FROM nft";
        std::string prev_result = ss.str();

        std::stringstream ss;
        auto writer = bustub::SimpleStreamWriter(ss, true);
        if (!bustub->ExecuteSqlTxn(query, writer, txn)) {
          txn_success = false;
        }

        if (txn_success) {
          if (ss.str() != prev_result) {
            fmt::print("ERROR: non repeatable read!\n");
            if (bustub_nft_num <= 100) {
              fmt::print("This is everything in your database:\n--- previous query ---\n{}\n--- this query ---\n{}\n",
                         prev_result, ss.str());
            }
            exit(1);
          }
          CheckTableLock(txn);
          bustub->txn_manager_->Commit(txn);
          metrics.TxnCommitted();
        } else {
          bustub->txn_manager_->Abort(txn);
          metrics.TxnAborted();
        }
      } else {
        bustub->txn_manager_->Abort(txn);
        metrics.TxnAborted();
      }
      delete txn;

      metrics.Report();

      if (bustub_nft_num > 1000) {
        // if NFT num is large, sleep this thread to avoid lock contention
        std::this_thread::sleep_for(std::chrono::seconds(3));
      }
    }

    total_metrics.ReportVerify(metrics.aborted_txn_cnt_, metrics.committed_txn_cnt_);
  }));

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
    if (ss.str() != fmt::format("{}\t\n", bustub_nft_num)) {
      fmt::print("unexpected result \"{}\" when verifying total nft count\n", ss.str());
      exit(1);
    }
  }

  {
    auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
    size_t cnt = 0;
    for (int i = 0; i < static_cast<int>(BUSTUB_TERRIER_CNT); i++) {
      std::stringstream ss;
      auto writer = bustub::SimpleStreamWriter(ss, true);
      auto sql = fmt::format("SELECT count(*) FROM nft WHERE terrier = {}", i);
      bustub->ExecuteSqlTxn(sql, writer, txn);
      cnt += std::stoi(ss.str());
    }

    {
      auto writer = bustub::SimpleStreamWriter(std::cout, true);
      auto sql = "SELECT count(*) FROM nft WHERE terrier = 0";
      std::cout << "SELECT count(*) FROM nft WHERE terrier = 0: ";
      bustub->ExecuteSqlTxn(sql, writer, txn);
    }

    bustub->txn_manager_->Commit(txn);
    delete txn;
    if (cnt != bustub_nft_num) {
      fmt::print("unexpected result \"{} != {}\" when verifying split nft count\n", cnt, bustub_nft_num);
      exit(1);
    }
  }

  total_metrics.Report();

  if (total_metrics.committed_verify_txn_cnt_ <= 3 || total_metrics.committed_update_txn_cnt_ < 3 ||
      total_metrics.committed_count_txn_cnt_ < 3) {
    fmt::print("too many txn are aborted");
    exit(1);
  }

  return 0;
}
