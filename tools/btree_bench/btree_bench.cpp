//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// btree_bench.cpp
//
// Identification: tools/btree_bench/btree_bench.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <mutex>  // NOLINT
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include <cpp_random_distributions/zipfian_int_distribution.h>

#include "argparse/argparse.hpp"
#include "binder/binder.h"
#include "buffer/buffer_pool_manager.h"
#include "buffer/lru_k_replacer.h"
#include "common/config.h"
#include "common/exception.h"
#include "common/rid.h"
#include "common/util/string_util.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/index/b_plus_tree.h"
#include "storage/index/generic_key.h"
#include "test_util.h"

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

static const size_t BUSTUB_READ_THREAD = 4;
static const size_t BUSTUB_WRITE_THREAD = 2;
// We should keep the BPM size large enough to hold all pages in memory, to minimize the dependency on P1.
// There will be roughly 500 leaf pages and tens of internal pages. Thus, 1024 should be enough.
static const size_t BUSTUB_BPM_SIZE = 1024;
static const size_t TOTAL_KEYS = 100000;
static const size_t KEY_MODIFY_RANGE = 2048;
static const size_t KEY_SCAN_RANGE = 2048;

// The reader mixes, run one after another over the same index with the writers throughout:
// point lookups, range scans, and half of each.
static const size_t NUM_WORKLOADS = 3;

using BTreeIndex = bustub::BPlusTree<bustub::GenericKey<8>, bustub::RID, bustub::GenericComparator<8>, -1>;

struct BTreeTotalMetrics {
  uint64_t write_cnt_{0};
  uint64_t read_cnt_{0};  // point lookups and range scans, one each
  uint64_t elapsed_ms_{0};
  uint64_t rss_start_kb_{0};
  uint64_t disk_bytes_{0};
  uint64_t bpm_bytes_{0};
  std::mutex mutex_;

  void ReportWrite(uint64_t write_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    write_cnt_ += write_cnt;
  }

  void ReportRead(uint64_t read_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    read_cnt_ += read_cnt;
  }

  // The autograder reads these lines by name.
  void Report() {
    auto write_per_sec = write_cnt_ / static_cast<double>(elapsed_ms_) * 1000;
    auto read_per_sec = read_cnt_ / static_cast<double>(elapsed_ms_) * 1000;

    fmt::print("<<< BEGIN\n");
    fmt::print("write: {}\n", write_per_sec);
    fmt::print("read: {}\n", read_per_sec);
    fmt::print("rss_start_kb: {}\n", rss_start_kb_);
    fmt::print("rss_peak_kb: {}\n", PeakRssKb());
    fmt::print("disk_bytes: {}\n", disk_bytes_);
    fmt::print("bpm_bytes: {}\n", bpm_bytes_);
    fmt::print(">>> END\n");
  }
};

struct BTreeMetrics {
  uint64_t start_time_{0};
  uint64_t last_report_at_{0};
  uint64_t last_cnt_{0};
  uint64_t cnt_{0};
  std::string reporter_;
  uint64_t duration_ms_;

  explicit BTreeMetrics(std::string reporter, uint64_t duration_ms)
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

// These keys will be deleted and inserted again
auto KeyWillVanish(size_t key) -> bool { return key % 7 == 0; }

// These keys will be overwritten to a new value
auto KeyWillChange(size_t key) -> bool { return key % 5 == 0; }

// Keys the writers never touch, so a reader can check them exactly
auto KeyIsStable(size_t key) -> bool { return !KeyWillVanish(key) && !KeyWillChange(key); }

// Point lookups over runs of consecutive keys starting at a random key.
auto LookupThread(BTreeIndex &index, size_t thread_id, size_t num_threads, uint64_t duration_ms) -> uint64_t {
  BTreeMetrics metrics(fmt::format("read  {:>2}", thread_id), duration_ms);
  metrics.Begin();

  size_t key_start = TOTAL_KEYS / num_threads * thread_id;
  size_t key_end = TOTAL_KEYS / num_threads * (thread_id + 1);
  std::random_device r;
  std::default_random_engine gen(r());
  std::uniform_int_distribution<size_t> dis(key_start, key_end - 1);

  bustub::GenericKey<8> index_key;
  std::vector<bustub::RID> rids;

  while (!metrics.ShouldFinish()) {
    auto base_key = dis(gen);
    size_t cnt = 0;
    for (auto key = base_key; key < key_end && cnt < KEY_MODIFY_RANGE; key++, cnt++) {
      rids.clear();
      index_key.SetFromInteger(key);
      index.GetValue(index_key, &rids);

      if (!KeyWillVanish(key) && rids.empty()) {
        std::string msg = fmt::format("key not found: {}", key);
        throw std::runtime_error(msg);
      }

      if (!KeyWillVanish(key) && !KeyWillChange(key)) {
        if (rids.size() != 1) {
          std::string msg = fmt::format("key not found: {}", key);
          throw std::runtime_error(msg);
        }
        if (static_cast<size_t>(rids[0].GetPageId()) != key || static_cast<size_t>(rids[0].GetSlotNum()) != key) {
          std::string msg = fmt::format("invalid data: {} -> {}", key, rids[0].Get());
          throw std::runtime_error(msg);
        }
      }
      metrics.Tick();
      metrics.Report();
    }
  }

  return metrics.cnt_;
}

// Deletes and re-inserts the vanishing keys, overwrites the changing keys.
// Range scans of KEY_SCAN_RANGE entries starting at a random key. Keys must come back in
// order, every stable key in the range must be there, stable values must be intact, and the
// scan may only end early when no stable key is left after it.
auto ScanThread(BTreeIndex &index, size_t thread_id, size_t num_threads, uint64_t duration_ms) -> uint64_t {
  BTreeMetrics metrics(fmt::format("scan  {:>2}", thread_id), duration_ms);
  metrics.Begin();

  size_t key_start = TOTAL_KEYS / num_threads * thread_id;
  size_t key_end = TOTAL_KEYS / num_threads * (thread_id + 1);
  std::random_device r;
  std::default_random_engine gen(r());
  std::uniform_int_distribution<size_t> dis(key_start, key_end - 1);

  bustub::GenericKey<8> index_key;

  while (!metrics.ShouldFinish()) {
    auto base_key = dis(gen);
    index_key.SetFromInteger(base_key);

    size_t expect = base_key;
    size_t cnt = 0;
    for (auto iter = index.Begin(index_key); !iter.IsEnd() && cnt < KEY_SCAN_RANGE; ++iter, cnt++) {
      auto [key, rid] = *iter;
      auto k = static_cast<size_t>(key.ToString());
      if (k < expect) {
        std::string msg = fmt::format("scan from {} returned {} after {}", base_key, k, expect - 1);
        throw std::runtime_error(msg);
      }
      for (; expect < k; expect++) {
        if (KeyIsStable(expect)) {
          std::string msg = fmt::format("scan from {} skipped key {}", base_key, expect);
          throw std::runtime_error(msg);
        }
      }
      if (KeyIsStable(k) && (static_cast<size_t>(rid.GetPageId()) != k || static_cast<size_t>(rid.GetSlotNum()) != k)) {
        std::string msg = fmt::format("invalid data: {} -> {}", k, rid.Get());
        throw std::runtime_error(msg);
      }
      expect = k + 1;
    }
    for (; cnt < KEY_SCAN_RANGE && expect < TOTAL_KEYS; expect++) {
      if (KeyIsStable(expect)) {
        std::string msg = fmt::format("scan from {} ended after {} entries, before key {}", base_key, cnt, expect);
        throw std::runtime_error(msg);
      }
    }
    metrics.Tick();
    metrics.Report();
  }

  return metrics.cnt_;
}

// Deletes and re-inserts the vanishing keys, overwrites the changing keys.
auto WriteThread(BTreeIndex &index, size_t thread_id, size_t num_threads, uint64_t duration_ms) -> uint64_t {
  BTreeMetrics metrics(fmt::format("write {:>2}", thread_id), duration_ms);
  metrics.Begin();

  size_t key_start = TOTAL_KEYS / num_threads * thread_id;
  size_t key_end = TOTAL_KEYS / num_threads * (thread_id + 1);
  std::random_device r;
  std::default_random_engine gen(r());
  std::uniform_int_distribution<size_t> dis(key_start, key_end - 1);

  bustub::GenericKey<8> index_key;
  bustub::RID rid;

  bool do_insert = false;

  while (!metrics.ShouldFinish()) {
    auto base_key = dis(gen);
    size_t cnt = 0;
    for (auto key = base_key; key < key_end && cnt < KEY_MODIFY_RANGE; key++, cnt++) {
      if (KeyWillVanish(key)) {
        uint32_t value = key;
        rid.Set(value, value);
        index_key.SetFromInteger(key);
        if (do_insert) {
          index.Insert(index_key, rid);
        } else {
          index.Remove(index_key);
        }
        metrics.Tick();
        metrics.Report();
      } else if (KeyWillChange(key)) {
        uint32_t value = key;
        rid.Set(value, dis(gen));
        index_key.SetFromInteger(key);
        index.Insert(index_key, rid);
        metrics.Tick();
        metrics.Report();
      }
    }
    do_insert = !do_insert;
  }

  return metrics.cnt_;
}

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  using bustub::AccessType;
  using bustub::BufferPoolManager;
  using bustub::DiskManagerUnlimitedMemory;
  using bustub::page_id_t;

  argparse::ArgumentParser program("bustub-btree-bench");
  program.add_argument("--duration").help("run btree bench for n milliseconds, split evenly over the three workloads");
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

  uint64_t memory_budget_kb = 0;
  if (program.present("--memory-budget-kb")) {
    memory_budget_kb = std::stoull(program.get("--memory-budget-kb"));
  }

  // Taken before anything is allocated, so the autograder can subtract the fixed cost of
  // the process from what the index and its buffer pool are responsible for.
  auto rss_start_kb = PeakRssKb();

  auto disk_manager = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(BUSTUB_BPM_SIZE, disk_manager.get());

  fmt::print(stderr, "[info] total_keys={}, duration_ms={}, bpm_size={}\n", TOTAL_KEYS, duration_ms, BUSTUB_BPM_SIZE);

  auto key_schema = bustub::ParseCreateStatement("a bigint");
  bustub::GenericComparator<8> comparator(key_schema.get());

  page_id_t page_id = bpm->NewPage();

  BTreeIndex index("foo_pk", page_id, bpm.get(), comparator);

  for (size_t key = 0; key < TOTAL_KEYS; key++) {
    bustub::GenericKey<8> index_key;
    bustub::RID rid;
    uint32_t value = key;
    rid.Set(value, value);
    index_key.SetFromInteger(key);
    index.Insert(index_key, rid);
  }

  fmt::print(stderr, "[info] benchmark start\n");

  BTreeTotalMetrics total_metrics;
  total_metrics.rss_start_kb_ = rss_start_kb;
  total_metrics.bpm_bytes_ = BUSTUB_BPM_SIZE * bustub::BUSTUB_PAGE_SIZE;

  // Watchdog. Without a budget this does nothing; with one, a submission that blows past
  // it is stopped rather than left to burn the rest of the benchmark's wall clock.
  std::atomic<bool> watchdog_stop{false};
  std::thread watchdog;
  if (memory_budget_kb != 0) {
    watchdog = std::thread([memory_budget_kb, &watchdog_stop] {
      while (!watchdog_stop.load()) {
        auto peak = PeakRssKb();
        if (peak > memory_budget_kb) {
          fmt::print(stderr, "[fatal] peak RSS {} kB exceeded the budget of {} kB, aborting\n", peak, memory_budget_kb);
          fmt::print(
              "<<< BEGIN\nwrite: 0\nread: 0\nrss_start_kb: 0\nrss_peak_kb: {}\ndisk_bytes: 0\nbpm_bytes: 0\n"
              ">>> END\n",
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

  // The run time is shared evenly by the three workloads.
  const uint64_t workload_ms = duration_ms / NUM_WORKLOADS;
  const char *workload_names[NUM_WORKLOADS] = {"lookup", "scan", "mixed"};
  const size_t lookup_threads[NUM_WORKLOADS] = {BUSTUB_READ_THREAD, 0, BUSTUB_READ_THREAD / 2};
  const size_t scan_threads[NUM_WORKLOADS] = {0, BUSTUB_READ_THREAD, BUSTUB_READ_THREAD / 2};

  for (size_t w = 0; w < NUM_WORKLOADS; w++) {
    fmt::print(stderr, "[info] workload {}: {} lookup, {} scan, {} write threads for {} ms\n", workload_names[w],
               lookup_threads[w], scan_threads[w], BUSTUB_WRITE_THREAD, workload_ms);

    auto start = ClockMs();
    std::vector<std::thread> threads;

    for (size_t thread_id = 0; thread_id < lookup_threads[w]; thread_id++) {
      threads.emplace_back([thread_id, num_threads = lookup_threads[w], &index, workload_ms, &total_metrics] {
        total_metrics.ReportRead(LookupThread(index, thread_id, num_threads, workload_ms));
      });
    }

    for (size_t thread_id = 0; thread_id < scan_threads[w]; thread_id++) {
      threads.emplace_back([thread_id, num_threads = scan_threads[w], &index, workload_ms, &total_metrics] {
        total_metrics.ReportRead(ScanThread(index, thread_id, num_threads, workload_ms));
      });
    }

    for (size_t thread_id = 0; thread_id < BUSTUB_WRITE_THREAD; thread_id++) {
      threads.emplace_back([thread_id, &index, workload_ms, &total_metrics] {
        total_metrics.ReportWrite(WriteThread(index, thread_id, BUSTUB_WRITE_THREAD, workload_ms));
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }

    total_metrics.elapsed_ms_ += ClockMs() - start;
  }

  watchdog_stop.store(true);
  if (watchdog.joinable()) {
    watchdog.join();
  }

  // Unlike bpm-bench, the disk size is not a parameter here: the tree decides how many pages
  // it needs, so it is read back from the disk once the workload has settled.
  total_metrics.disk_bytes_ = disk_manager->GetMemoryUsage();
  total_metrics.Report();

  return 0;
}
