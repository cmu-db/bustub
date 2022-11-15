#include <chrono>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "binder/binder.h"
#include "common/bustub_instance.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "concurrency/transaction.h"
#include "concurrency/transaction_manager.h"
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
static const size_t BUSTUB_TERRIER_BENCH_DURATION = 10000;

struct TerrierMetrics {
  uint64_t start_time_{0};
  uint64_t last_report_at_{0};
  uint64_t last_total_txn_cnt_{0};
  uint64_t last_aborted_txn_cnt_{0};
  uint64_t total_txn_cnt_{0};
  uint64_t aborted_txn_cnt_{0};
  std::string reporter_;

  explicit TerrierMetrics(std::string reporter) : reporter_(std::move(reporter)) {}

  void TxnAborted() { aborted_txn_cnt_ += 1; }

  void TxnCommitted() { total_txn_cnt_ += 1; }

  void Begin() { start_time_ = ClockMs(); }

  void Report() {
    auto now = ClockMs();
    auto elsped = now - start_time_;
    if (elsped - last_report_at_ > 1000) {
      fmt::print("{}: total_txn={:<5} total_aborted_txn={:<5} throughput={:<6.3} avg_throughput={:<6.3}\n", reporter_,
                 total_txn_cnt_, aborted_txn_cnt_,
                 (total_txn_cnt_ - last_total_txn_cnt_) / static_cast<double>(elsped - last_report_at_) * 1000,
                 total_txn_cnt_ / static_cast<double>(elsped) * 1000);
      last_report_at_ = elsped;
      last_total_txn_cnt_ = total_txn_cnt_;
    }
  }

  auto ShouldFinish() -> bool {
    auto now = ClockMs();
    return now - start_time_ > BUSTUB_TERRIER_BENCH_DURATION;
  }
};
// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
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

  if (enable_index) {
    auto schema = "CREATE INDEX nftid nft(id;";
    std::cerr << "x: create index" << std::endl;
    bustub->ExecuteSql(schema, writer);
  }

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

  auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
  bustub->ExecuteSqlTxn(query, writer, txn);
  bustub->txn_manager_->Commit(txn);
  delete txn;

  std::cerr << "x: benchmark start" << std::endl;

  std::vector<std::thread> threads;

#ifdef TERRIER_BENCH_ENABLE_UPDATE
  bool enable_update = true;
#else
  bool enable_update = false;
#endif

  for (size_t thread_id = 0; thread_id < BUSTUB_TERRIER_THREAD; thread_id++) {
    threads.emplace_back(std::thread([thread_id, &bustub, enable_update] {
      const size_t nft_range_size = BUSTUB_NFT_NUM / BUSTUB_TERRIER_THREAD;
      const size_t nft_range_begin = thread_id * nft_range_size;
      const size_t nft_range_end = (thread_id + 1) * nft_range_size;
      std::random_device r;
      std::default_random_engine gen(r());
      std::uniform_int_distribution<int> nft_uniform_dist(nft_range_begin, nft_range_end - 1);
      std::uniform_int_distribution<int> terrier_uniform_dist(0, BUSTUB_TERRIER_CNT - 1);

      TerrierMetrics metrics(fmt::format("Update {}", thread_id));
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
    }));
  }

  for (size_t thread_id = 0; thread_id < BUSTUB_TERRIER_THREAD; thread_id++) {
    threads.emplace_back(std::thread([thread_id, &bustub] {
      std::random_device r;
      std::default_random_engine gen(r());
      std::uniform_int_distribution<int> terrier_uniform_dist(0, BUSTUB_TERRIER_CNT - 1);

      TerrierMetrics metrics(fmt::format(" Count {}", thread_id));
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
    }));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  {
    auto txn = bustub->txn_manager_->Begin(nullptr, bustub::IsolationLevel::REPEATABLE_READ);
    auto writer = bustub::SimpleStreamWriter(std::cerr);
    std::string query = fmt::format("SELECT count(*) FROM terrier");
    bustub->ExecuteSqlTxn(query, writer, txn);
  }

  return 0;
}
