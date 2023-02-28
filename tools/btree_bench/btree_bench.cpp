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
#include "common/rid.h"
#include "common/util/string_util.h"
#include "fmt/format.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/index/b_plus_tree.h"
#include "storage/index/generic_key.h"
#include "test_util.h"

#include <sys/time.h>

auto ClockMs() -> uint64_t {
  struct timeval tm;
  gettimeofday(&tm, nullptr);
  return static_cast<uint64_t>(tm.tv_sec * 1000) + static_cast<uint64_t>(tm.tv_usec / 1000);
}

static const size_t BUSTUB_READ_THREAD = 4;
static const size_t BUSTUB_WRITE_THREAD = 2;
static const size_t LRU_K_SIZE = 4;
static const size_t BUSTUB_BPM_SIZE = 256;
static const size_t TOTAL_KEYS = 100000;
static const size_t KEY_MODIFY_RANGE = 2048;

struct BTreeTotalMetrics {
  uint64_t write_cnt_{0};
  uint64_t read_cnt_{0};
  uint64_t start_time_{0};
  std::mutex mutex_;

  void Begin() { start_time_ = ClockMs(); }

  void ReportWrite(uint64_t scan_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    write_cnt_ += scan_cnt;
  }

  void ReportRead(uint64_t get_cnt) {
    std::unique_lock<std::mutex> l(mutex_);
    read_cnt_ += get_cnt;
  }

  void Report() {
    auto now = ClockMs();
    auto elsped = now - start_time_;
    auto write_per_sec = write_cnt_ / static_cast<double>(elsped) * 1000;
    auto read_per_sec = read_cnt_ / static_cast<double>(elsped) * 1000;

    fmt::print("<<< BEGIN\n");
    fmt::print("write: {}\n", write_per_sec);
    fmt::print("read: {}\n", read_per_sec);
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

// These keys will be deleted and inserted again
auto KeyWillVanish(size_t key) -> bool { return key % 7 == 0; }

// These keys will be overwritten to a new value
auto KeyWillChange(size_t key) -> bool { return key % 5 == 0; }

// NOLINTNEXTLINE
auto main(int argc, char **argv) -> int {
  using bustub::AccessType;
  using bustub::BufferPoolManager;
  using bustub::DiskManagerUnlimitedMemory;
  using bustub::page_id_t;

  argparse::ArgumentParser program("bustub-btree-bench");
  program.add_argument("--duration").help("run btree bench for n milliseconds");

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

  auto disk_manager = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(BUSTUB_BPM_SIZE, disk_manager.get(), LRU_K_SIZE);

  fmt::print(stderr, "[info] total_keys={}, duration_ms={}, lru_k_size={}, bpm_size={}\n", TOTAL_KEYS, duration_ms,
             LRU_K_SIZE, BUSTUB_BPM_SIZE);

  auto key_schema = bustub::ParseCreateStatement("a bigint");
  bustub::GenericComparator<8> comparator(key_schema.get());

  page_id_t page_id;
  auto header_page = bpm->NewPageGuarded(&page_id);

  bustub::BPlusTree<bustub::GenericKey<8>, bustub::RID, bustub::GenericComparator<8>> index("foo_pk", page_id,
                                                                                            bpm.get(), comparator);

  for (size_t key = 0; key < TOTAL_KEYS; key++) {
    bustub::GenericKey<8> index_key;
    bustub::RID rid;
    uint32_t value = key;
    rid.Set(value, value);
    index_key.SetFromInteger(key);
    index.Insert(index_key, rid, nullptr);
  }

  fmt::print(stderr, "[info] benchmark start\n");

  BTreeTotalMetrics total_metrics;
  total_metrics.Begin();

  std::vector<std::thread> threads;

  for (size_t thread_id = 0; thread_id < BUSTUB_READ_THREAD; thread_id++) {
    threads.emplace_back(std::thread([thread_id, &index, duration_ms, &total_metrics] {
      BTreeMetrics metrics(fmt::format("read  {:>2}", thread_id), duration_ms);
      metrics.Begin();

      size_t key_start = TOTAL_KEYS / BUSTUB_READ_THREAD * thread_id;
      size_t key_end = TOTAL_KEYS / BUSTUB_READ_THREAD * (thread_id + 1);
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

      total_metrics.ReportRead(metrics.cnt_);
    }));
  }

  for (size_t thread_id = 0; thread_id < BUSTUB_WRITE_THREAD; thread_id++) {
    threads.emplace_back(std::thread([thread_id, &index, duration_ms, &total_metrics] {
      BTreeMetrics metrics(fmt::format("write {:>2}", thread_id), duration_ms);
      metrics.Begin();

      size_t key_start = TOTAL_KEYS / BUSTUB_WRITE_THREAD * thread_id;
      size_t key_end = TOTAL_KEYS / BUSTUB_WRITE_THREAD * (thread_id + 1);
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
              index.Insert(index_key, rid, nullptr);
            } else {
              index.Remove(index_key, nullptr);
            }
            metrics.Tick();
            metrics.Report();
          } else if (KeyWillChange(key)) {
            uint32_t value = key;
            rid.Set(value, dis(gen));
            index_key.SetFromInteger(key);
            index.Insert(index_key, rid, nullptr);
            metrics.Tick();
            metrics.Report();
          }
        }
        do_insert = !do_insert;
      }

      total_metrics.ReportWrite(metrics.cnt_);
    }));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  total_metrics.Report();

  return 0;
}
