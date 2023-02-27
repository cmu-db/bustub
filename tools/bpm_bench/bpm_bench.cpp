#include <chrono>
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
#include "common/util/string_util.h"
#include "fmt/core.h"
#include "fmt/std.h"
#include "storage/disk/disk_manager_memory.h"

#include <sys/time.h>

auto ClockMs() -> uint64_t {
  struct timeval tm;
  gettimeofday(&tm, nullptr);
  return static_cast<uint64_t>(tm.tv_sec * 1000) + static_cast<uint64_t>(tm.tv_usec / 1000);
}

static const size_t BUSTUB_SCAN_THREAD = 8;
static const size_t BUSTUB_GET_THREAD = 8;
static const size_t LRU_K_SIZE = 16;
static const size_t BUSTUB_PAGE_CNT = 6400;
static const size_t BUSTUB_BPM_SIZE = 64;

struct BpmTotalMetrics {
  uint64_t scan_cnt_{0};
  uint64_t get_cnt_{0};
  uint64_t start_time_{0};
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
    auto elsped = now - start_time_;
    auto scan_per_sec = scan_cnt_ / static_cast<double>(elsped) * 1000;
    auto get_per_sec = get_cnt_ / static_cast<double>(elsped) * 1000;

    fmt::print("<<< BEGIN\n");
    fmt::print("scan: {}\n", scan_per_sec);
    fmt::print("get: {}\n", get_per_sec);
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
    auto elsped = now - start_time_;
    if (elsped - last_report_at_ > 1000) {
      fmt::print(stderr, "[{:5.2f}] {}: total_cnt={:<10} throughput={:<10.3f} avg_throughput={:<10.3f}\n",
                 elsped / 1000.0, reporter_, cnt_,
                 (cnt_ - last_cnt_) / static_cast<double>(elsped - last_report_at_) * 1000,
                 cnt_ / static_cast<double>(elsped) * 1000);
      last_report_at_ = elsped;
      last_cnt_ = cnt_;
    }
  }

  auto ShouldFinish() -> bool {
    auto now = ClockMs();
    return now - start_time_ > duration_ms_;
  }
};

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  using bustub::AccessType;
  using bustub::BufferPoolManager;
  using bustub::DiskManagerUnlimitedMemory;
  using bustub::page_id_t;

  argparse::ArgumentParser program("bustub-bpm-bench");
  program.add_argument("--duration").help("run bpm bench for n milliseconds");
  program.add_argument("--latency").help("set disk latency to n milliseconds");

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

  uint64_t latency_ms = 0;
  if (program.present("--latency")) {
    latency_ms = std::stoi(program.get("--latency"));
  }

  auto disk_manager = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(BUSTUB_BPM_SIZE, disk_manager.get(), LRU_K_SIZE);
  std::vector<page_id_t> page_ids;

  fmt::print(stderr, "[info] total_page={}, duration_ms={}, latency_ms={}, lru_k_size={}, bpm_size={}\n",
             BUSTUB_PAGE_CNT, duration_ms, latency_ms, LRU_K_SIZE, BUSTUB_BPM_SIZE);

  for (size_t i = 0; i < BUSTUB_PAGE_CNT; i++) {
    page_id_t page_id;
    auto *page = bpm->NewPage(&page_id);
    if (page == nullptr) {
      throw std::runtime_error("new page failed");
    }
    char &ch = page->GetData()[i % 1024];
    ch = 1;

    bpm->UnpinPage(page_id, true);
    page_ids.push_back(page_id);
  }

  // enable disk latency after creating all pages
  disk_manager->SetLatency(latency_ms);

  fmt::print(stderr, "[info] benchmark start\n");

  BpmTotalMetrics total_metrics;
  total_metrics.Begin();

  std::vector<std::thread> threads;

  for (size_t thread_id = 0; thread_id < BUSTUB_SCAN_THREAD; thread_id++) {
    threads.emplace_back(std::thread([thread_id, &page_ids, &bpm, duration_ms, &total_metrics] {
      BpmMetrics metrics(fmt::format("scan {:>2}", thread_id), duration_ms);
      metrics.Begin();

      size_t page_idx = BUSTUB_PAGE_CNT * thread_id / BUSTUB_SCAN_THREAD;

      while (!metrics.ShouldFinish()) {
        auto *page = bpm->FetchPage(page_ids[page_idx], AccessType::Scan);
        if (page == nullptr) {
          continue;
        }

        char &ch = page->GetData()[page_idx % 1024];
        page->WLatch();
        ch += 1;
        if (ch == 0) {
          ch = 1;
        }
        page->WUnlatch();

        bpm->UnpinPage(page->GetPageId(), true, AccessType::Scan);
        page_idx = (page_idx + 1) % BUSTUB_PAGE_CNT;
        metrics.Tick();
        metrics.Report();
      }

      total_metrics.ReportScan(metrics.cnt_);
    }));
  }

  for (size_t thread_id = 0; thread_id < BUSTUB_GET_THREAD; thread_id++) {
    threads.emplace_back(std::thread([thread_id, &page_ids, &bpm, duration_ms, &total_metrics] {
      std::random_device r;
      std::default_random_engine gen(r());
      zipfian_int_distribution<size_t> dist(0, BUSTUB_PAGE_CNT - 1, 0.8);

      BpmMetrics metrics(fmt::format("get  {:>2}", thread_id), duration_ms);
      metrics.Begin();

      while (!metrics.ShouldFinish()) {
        auto page_idx = dist(gen);
        auto *page = bpm->FetchPage(page_ids[page_idx], AccessType::Get);
        if (page == nullptr) {
          continue;
        }

        page->RLatch();
        char ch = page->GetData()[page_idx % 1024];
        page->RUnlatch();
        if (ch == 0) {
          throw std::runtime_error("invalid data");
        }

        bpm->UnpinPage(page->GetPageId(), false, AccessType::Get);
        metrics.Tick();
        metrics.Report();
      }

      total_metrics.ReportGet(metrics.cnt_);
    }));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  total_metrics.Report();

  return 0;
}
