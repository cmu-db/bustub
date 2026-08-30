//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// bpm_bench.cpp
//
// Identification: tools/bpm_bench/bpm_bench.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>  // NOLINT
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <cpp_random_distributions/zipfian_int_distribution.h>

#include "argparse/argparse.hpp"
#include "binder/binder.h"
#include "buffer/buffer_pool_manager.h"
#include "common/config.h"
#include "common/exception.h"
#include "common/util/string_util.h"
#include "fmt/core.h"
#include "fmt/std.h"
#include "storage/disk/disk_manager_memory.h"

#include <sys/time.h>

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__linux__)
#define BUSTUB_HAS_RUSAGE
#include <sys/resource.h>
#endif

auto ClockMs() -> uint64_t {
  struct timeval tm;
  gettimeofday(&tm, nullptr);
  return static_cast<uint64_t>(tm.tv_sec * 1000) + static_cast<uint64_t>(tm.tv_usec / 1000);
}

/// Peak resident set size in kB, or 0 where it cannot be determined.
auto PeakRssKb() -> uint64_t {
#if defined(BUSTUB_HAS_RUSAGE)
  struct rusage usage {};
  if (getrusage(RUSAGE_SELF, &usage) != 0) {
    return 0;
  }
#if defined(__APPLE__) && defined(__MACH__)
  return static_cast<uint64_t>(usage.ru_maxrss) / 1024;
#else
  return static_cast<uint64_t>(usage.ru_maxrss);
#endif
#else
  return 0;
#endif
}

struct BpmTotalMetrics {
  uint64_t scan_cnt_{0};
  uint64_t get_cnt_{0};
  uint64_t start_time_{0};
  uint64_t rss_start_kb_{0};
  uint64_t disk_bytes_{0};
  uint64_t db_pages_{0};
  uint64_t bpm_bytes_{0};
  std::mutex mutex_;

  void Begin() { start_time_ = ClockMs(); }

  void ReportScan(uint64_t scan_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    scan_cnt_ += scan_cnt;
  }

  void ReportGet(uint64_t get_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    get_cnt_ += get_cnt;
  }

  void Report() {
    auto now = ClockMs();
    auto elapsed = now - start_time_;
    auto scan_per_sec = scan_cnt_ / static_cast<double>(elapsed) * 1000;
    auto get_per_sec = get_cnt_ / static_cast<double>(elapsed) * 1000;

    fmt::print("<<< BEGIN\n");
    fmt::print("scan: {}\n", scan_per_sec);
    fmt::print("get: {}\n", get_per_sec);
    fmt::print("rss_start_kb: {}\n", rss_start_kb_);
    fmt::print("rss_peak_kb: {}\n", PeakRssKb());
    fmt::print("disk_bytes: {}\n", disk_bytes_);
    fmt::print("db_pages: {}\n", db_pages_);
    fmt::print("bpm_bytes: {}\n", bpm_bytes_);
    fmt::print(">>> END\n");
  }
};

struct BpmMetrics {
  uint64_t start_time_{0};
  uint64_t last_report_at_{0};
  uint64_t last_cnt_{0};
  uint64_t cnt_{0};
  std::string reporter_;
  uint64_t duration_ms_;

  explicit BpmMetrics(std::string reporter, uint64_t duration_ms)
      : reporter_(std::move(reporter)), duration_ms_(duration_ms) {}

  void Tick() { cnt_ += 1; }

  void Begin() { start_time_ = ClockMs(); }

  void Report() {
    auto now = ClockMs();
    auto elapsed = now - start_time_;
    if (elapsed - last_report_at_ > 1000) {
      fmt::print(stderr, "[{:5.2f}] {}: total_cnt={:<10} throughput={:<10.3f} avg_throughput={:<10.3f}\n",
                 elapsed / 1000.0, reporter_, cnt_,
                 (cnt_ - last_cnt_) / static_cast<double>(elapsed - last_report_at_) * 1000,
                 cnt_ / static_cast<double>(elapsed) * 1000);
      last_report_at_ = elapsed;
      last_cnt_ = cnt_;
    }
  }

  auto ShouldFinish() -> bool {
    auto now = ClockMs();
    return now - start_time_ > duration_ms_;
  }
};

struct BusTubBenchPageHeader {
  uint64_t seed_;
  uint64_t page_id_;
  char data_[0];
};

/// Modify the page and save some data inside
auto ModifyPage(char *data, size_t page_idx, uint64_t seed) -> void {
  auto *pg = reinterpret_cast<BusTubBenchPageHeader *>(data);
  pg->seed_ = seed;
  pg->page_id_ = page_idx;
  pg->data_[pg->seed_ % 4000] = pg->seed_ % 256;
}

/// Check the page and verify the data inside
auto CheckPageConsistentNoSeed(const char *data, size_t page_idx) -> void {
  const auto *pg = reinterpret_cast<const BusTubBenchPageHeader *>(data);
  if (pg->page_id_ != page_idx) {
    fmt::println(stderr, "page header not consistent: page_id_={} page_idx={}", pg->page_id_, page_idx);
    std::terminate();
  }
  auto left = static_cast<unsigned int>(static_cast<unsigned char>(pg->data_[pg->seed_ % 4000]));
  auto right = static_cast<unsigned int>(pg->seed_ % 256);
  if (left != right) {
    fmt::println(stderr, "page content not consistent: data_[{}]={} seed_ % 256={}", pg->seed_ % 4000, left, right);
    std::terminate();
  }
}

/// Check the page and verify the data inside
auto CheckPageConsistent(const char *data, size_t page_idx, uint64_t seed) -> void {
  const auto *pg = reinterpret_cast<const BusTubBenchPageHeader *>(data);
  if (pg->seed_ != seed) {
    fmt::println(stderr, "page seed not consistent: seed_={} seed={}", pg->seed_, seed);
    std::terminate();
  }
  CheckPageConsistentNoSeed(data, page_idx);
}

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  using bustub::AccessType;
  using bustub::BufferPoolManager;
  using bustub::DiskManagerUnlimitedMemory;
  using bustub::page_id_t;

  argparse::ArgumentParser program("bustub-bpm-bench");
  program.add_argument("--duration").help("run bpm bench for n milliseconds");
  program.add_argument("--latency").help("enable disk latency");
  program.add_argument("--scan-thread-n").help("number of scan threads");
  program.add_argument("--get-thread-n").help("number of lookup threads");
  program.add_argument("--bpm-size").help("buffer pool size");
  program.add_argument("--db-size").help("number of pages");
  program.add_argument("--lru-k-size").help("lru-k size");
  program.add_argument("--memory-budget-kb")
      .help("abort if peak RSS exceeds this many kB; 0 or absent means report only");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  uint64_t duration_ms = 30000;
  if (program.present("--duration")) {
    duration_ms = std::stoi(program.get("--duration"));
  }

  uint64_t enable_latency = 0;
  if (program.present("--latency")) {
    enable_latency = std::stoi(program.get("--latency"));
  }

  uint64_t scan_thread_n = 8;
  if (program.present("--scan-thread-n")) {
    scan_thread_n = std::stoi(program.get("--scan-thread-n"));
  }

  uint64_t get_thread_n = 8;
  if (program.present("--get-thread-n")) {
    get_thread_n = std::stoi(program.get("--get-thread-n"));
  }

  uint64_t bustub_page_cnt = 6400;
  if (program.present("--db-size")) {
    bustub_page_cnt = std::stoi(program.get("--db-size"));
  }

  uint64_t bustub_bpm_size = 64;
  if (program.present("--bpm-size")) {
    bustub_bpm_size = std::stoi(program.get("--bpm-size"));
  }

  uint64_t lru_k_size = 16;
  if (program.present("--lru-k-size")) {
    lru_k_size = std::stoi(program.get("--lru-k-size"));
  }

  uint64_t memory_budget_kb = 0;
  if (program.present("--memory-budget-kb")) {
    memory_budget_kb = std::stoull(program.get("--memory-budget-kb"));
  }

  // Taken before anything is allocated, so the autograder can subtract the fixed cost of
  // the process from what the buffer pool is responsible for.
  auto rss_start_kb = PeakRssKb();

  auto disk_manager = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(bustub_bpm_size, disk_manager.get());
  std::vector<page_id_t> page_ids;

  fmt::print(stderr,
             "[info] total_page={}, duration_ms={}, latency={}, lru_k_size={}, bpm_size={}, scan_thread_cnt={}, "
             "get_thread_cnt={}\n",
             bustub_page_cnt, duration_ms, enable_latency, lru_k_size, bustub_bpm_size, scan_thread_n, get_thread_n);

  for (size_t i = 0; i < bustub_page_cnt; i++) {
    page_id_t page_id = bpm->NewPage();
    {
      auto guard = bpm->WritePage(page_id);
      ModifyPage(guard.GetDataMut(), i, 0);
    }
    page_ids.push_back(page_id);
  }

  // enable disk latency after creating all pages
  disk_manager->EnableLatencySimulator(enable_latency != 0);

  fmt::print(stderr, "[info] benchmark start\n");

  BpmTotalMetrics total_metrics;
  total_metrics.rss_start_kb_ = rss_start_kb;
  total_metrics.disk_bytes_ = disk_manager->GetMemoryUsage();
  total_metrics.db_pages_ = bustub_page_cnt;
  total_metrics.bpm_bytes_ = bustub_bpm_size * bustub::BUSTUB_PAGE_SIZE;
  total_metrics.Begin();

  // Watchdog. Without a budget this does nothing; with one, a submission that blows past
  // it is stopped rather than left to burn the rest of the benchmark's wall clock.
  std::atomic<bool> watchdog_stop{false};
  std::thread watchdog;
  if (memory_budget_kb != 0) {
    watchdog = std::thread([memory_budget_kb, &watchdog_stop] {
      while (!watchdog_stop.load()) {
        auto peak = PeakRssKb();
        if (peak > memory_budget_kb) {
          fmt::print(stderr, "[fatal] peak RSS {} kB exceeded the budget of {} kB, aborting\n", peak,
                     memory_budget_kb);
          fmt::print(
              "<<< BEGIN\nscan: 0\nget: 0\nrss_start_kb: 0\nrss_peak_kb: {}\ndisk_bytes: 0\ndb_pages: 0\n"
              "bpm_bytes: 0\n>>> END\n",
              peak);
          std::fflush(stdout);
          // _Exit rather than exit: the benchmark threads are still running and unwinding
          // through their destructors from here would be a data race.
          std::_Exit(1);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    });
  }

  std::vector<std::thread> threads;
  using ModifyRecord = std::unordered_map<page_id_t, uint64_t>;

  for (size_t thread_id = 0; thread_id < scan_thread_n; thread_id++) {
    threads.emplace_back([bustub_page_cnt, scan_thread_n, thread_id, &page_ids, &bpm, duration_ms, &total_metrics] {
      BpmMetrics metrics(fmt::format("scan {:>2}", thread_id), duration_ms);
      metrics.Begin();

      size_t page_idx_start = bustub_page_cnt * thread_id / scan_thread_n;
      size_t page_idx_end = bustub_page_cnt * (thread_id + 1) / scan_thread_n;
      size_t page_idx = page_idx_start;

      while (!metrics.ShouldFinish()) {
        {
          auto page = bpm->ReadPage(page_ids[page_idx], AccessType::Scan);
          CheckPageConsistentNoSeed(page.GetData(), page_idx);
        }

        page_idx += 1;
        if (page_idx >= page_idx_end) {
          page_idx = page_idx_start;
        }
        metrics.Tick();
        metrics.Report();
      }

      total_metrics.ReportScan(metrics.cnt_);
    });
  }

  for (size_t thread_id = 0; thread_id < get_thread_n; thread_id++) {
    threads.emplace_back([thread_id, &page_ids, &bpm, bustub_page_cnt, get_thread_n, duration_ms, &total_metrics] {
      std::random_device r;
      std::default_random_engine gen(r());
      zipfian_int_distribution<size_t> dist(0, bustub_page_cnt - 1, 0.8);
      ModifyRecord records;

      BpmMetrics metrics(fmt::format("get {:>2}", thread_id), duration_ms);
      metrics.Begin();

      while (!metrics.ShouldFinish()) {
        auto rand = dist(gen);
        auto page_idx = std::min(rand / get_thread_n * get_thread_n + thread_id, bustub_page_cnt - 1);
        {
          auto page = bpm->WritePage(page_ids[page_idx], AccessType::Lookup);
          auto &seed = records[page_idx];
          CheckPageConsistent(page.GetData(), page_idx, seed);
          seed = seed + 1;
          ModifyPage(page.GetDataMut(), page_idx, seed);
        }

        metrics.Tick();
        metrics.Report();
      }

      total_metrics.ReportGet(metrics.cnt_);
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  watchdog_stop.store(true);
  if (watchdog.joinable()) {
    watchdog.join();
  }

  total_metrics.Report();

  return 0;
}
