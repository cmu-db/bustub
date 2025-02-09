//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree_contention_test.cpp
//
// Identification: test/storage/b_plus_tree_contention_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

/**
 * b_plus_tree_contention_test.cpp
 */

#include <chrono>  // NOLINT
#include <cstdio>
#include <functional>
#include <future>  // NOLINT
#include <iostream>
#include <thread>  // NOLINT

#include "buffer/buffer_pool_manager.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/index/b_plus_tree.h"
#include "test_util.h"  // NOLINT

namespace bustub {

size_t BPlusTreeLockBenchmarkCall(size_t num_threads, bool with_global_mutex) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  // create lightweight BPM
  const size_t bpm_size = 256 << 10;  // 1GB
  DiskManagerMemory disk_manager(bpm_size);
  BufferPoolManager bpm(bpm_size, &disk_manager);

  // allocate header_page
  page_id_t page_id = bpm.NewPage();

  // create b+ tree
  const int node_size = 20;
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", page_id, &bpm, comparator, node_size, node_size);

  std::vector<std::thread> threads;

  const int keys_per_stride = 160000 / num_threads;
  const int key_stride = 6400000;
  std::mutex mtx;

  auto insert_keys = [&](const int64_t start_key, const int64_t end_key) {
    GenericKey<8> index_key;
    RID rid;
    for (auto key = start_key; key < end_key; key++) {
      int64_t value = key & 0xFFFFFFFF;
      rid.Set(static_cast<int32_t>(key >> 32), value);
      index_key.SetFromInteger(key);
      if (with_global_mutex) {
        mtx.lock();
      }
      tree.Insert(index_key, rid);
      if (with_global_mutex) {
        mtx.unlock();
      }
    }
  };

  // Measure
  auto start_time = std::chrono::system_clock::now();
  for (size_t i = 0; i < num_threads; i++) {
    const auto start_key = i * key_stride;
    threads.emplace_back(insert_keys, start_key, start_key + keys_per_stride);
  }
  for (auto &thread : threads) {
    thread.join();
  }
  auto end_time = std::chrono::system_clock::now();

  return std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
}

TEST(BPlusTreeContentionTest, BPlusTreeContentionBenchmark) {  // NOLINT
  std::cout << "This test will see how your B+ tree performance differs with and without contention." << std::endl;
  std::cout << "If your submission timeout, segfault, or didn't implement lock crabbing, we will manually deduct all "
               "concurrent test points (maximum 25)."
            << std::endl;

  std::vector<size_t> time_ms_with_mutex;
  std::vector<size_t> time_ms_wo_mutex;
  for (size_t iter = 0; iter < 10; iter++) {
    bool enable_mutex = iter % 2 == 0;
    auto time = BPlusTreeLockBenchmarkCall(4, enable_mutex);
    if (enable_mutex) {
      time_ms_with_mutex.push_back(time);
    } else {
      time_ms_wo_mutex.push_back(time);
    }
  }

  std::cout << "<<< BEGIN" << std::endl;
  std::cout << "Multithreaded Access Time: ";
  double sum_1 = 0;
  double sum_2 = 0;
  for (auto x : time_ms_wo_mutex) {
    std::cout << x << " ";
    sum_1 += x;
  }
  std::cout << std::endl;

  std::cout << "Serialized Access Time: ";
  for (auto x : time_ms_with_mutex) {
    std::cout << x << " ";
    sum_2 += x;
  }
  std::cout << std::endl;
  double speedup = sum_2 / sum_1;
  std::cout << "Speedup: " << speedup << std::endl;
  std::cout << ">>> END" << std::endl;
  std::cout << "If your above data is an outlier in all submissions (based on statistics and probably some "
               "machine-learning), TAs will manually inspect your code to ensure you are implementing lock crabbing "
               "correctly."
            << std::endl;
}

}  // namespace bustub
