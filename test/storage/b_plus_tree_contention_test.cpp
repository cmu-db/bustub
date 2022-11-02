/**
 * b_plus_tree_contention_test.cpp
 */

#include <chrono>  // NOLINT
#include <cstdio>
#include <functional>
#include <future>  // NOLINT
#include <iostream>
#include <thread>  // NOLINT

#include "buffer/buffer_pool_manager_instance.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/index/b_plus_tree.h"
#include "test_util.h"  // NOLINT

namespace bustub {

bool BPlusTreeLockBenchmarkCall(size_t num_threads, int leaf_node_size, bool with_global_mutex) {
  bool success = true;
  std::vector<int64_t> insert_keys;

  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());
  auto *disk_manager = new DiskManagerMemory(256 << 10);  // 1GB
  BufferPoolManager *bpm = new BufferPoolManagerInstance(64, disk_manager);
  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", bpm, comparator, leaf_node_size, 10);
  // create and fetch header_page
  page_id_t page_id;
  auto *header_page = bpm->NewPage(&page_id);
  (void)header_page;

  std::vector<std::thread> threads;

  const int keys_per_thread = 20000 / num_threads;
  const int keys_stride = 100000;
  std::mutex mtx;

  for (size_t i = 0; i < num_threads; i++) {
    auto func = [&tree, &mtx, i, keys_per_thread, with_global_mutex]() {
      GenericKey<8> index_key;
      RID rid;
      auto *transaction = new Transaction(static_cast<txn_id_t>(i + 1));
      const auto end_key = keys_stride * i + keys_per_thread;
      for (auto key = i * keys_stride; key < end_key; key++) {
        int64_t value = key & 0xFFFFFFFF;
        rid.Set(static_cast<int32_t>(key >> 32), value);
        index_key.SetFromInteger(key);
        if (with_global_mutex) {
          mtx.lock();
        }
        tree.Insert(index_key, rid, transaction);
        if (with_global_mutex) {
          mtx.unlock();
        }
      }
      delete transaction;
    };
    auto t = std::thread(std::move(func));
    threads.emplace_back(std::move(t));
  }

  for (auto &thread : threads) {
    thread.join();
  }

  bpm->UnpinPage(HEADER_PAGE_ID, true);
  delete disk_manager;
  delete bpm;

  return success;
}

TEST(BPlusTreeTest, BPlusTreeContentionBenchmark) {  // NOLINT
  std::vector<size_t> time_ms_with_mutex;
  std::vector<size_t> time_ms_wo_mutex;
  for (size_t iter = 0; iter < 20; iter++) {
    bool enable_mutex = iter % 2 == 0;
    auto clock_start = std::chrono::system_clock::now();
    ASSERT_TRUE(BPlusTreeLockBenchmarkCall(32, 2, enable_mutex));
    auto clock_end = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start);
    if (enable_mutex) {
      time_ms_with_mutex.push_back(dur.count());
    } else {
      time_ms_wo_mutex.push_back(dur.count());
    }
  }
  std::cout << "This test will see how your B+ tree performance differs with and without contention." << std::endl;
  std::cout << "<<< BEGIN" << std::endl;
  std::cout << "Normal Access Time: ";
  double ratio_1 = 0;
  double ratio_2 = 0;
  for (auto x : time_ms_wo_mutex) {
    std::cout << x << " ";
    ratio_1 += x;
  }
  std::cout << std::endl;

  std::cout << "Serialized Access Time: ";
  for (auto x : time_ms_with_mutex) {
    std::cout << x << " ";
    ratio_2 += x;
  }
  std::cout << std::endl;
  std::cout << "Ratio: " << ratio_1 / ratio_2 << std::endl;
  std::cout << ">>> END" << std::endl;
  std::cout << "If your above data is an outlier in all submissions (based on statistics and probably some "
               "machine-learning), TAs will manually inspect your code to ensure you are implementing lock crabbing "
               "correctly."
            << std::endl;
}

TEST(BPlusTreeTest, BPlusTreeContentionBenchmark2) {  // NOLINT
  std::vector<size_t> time_ms_with_mutex;
  std::vector<size_t> time_ms_wo_mutex;
  for (size_t iter = 0; iter < 20; iter++) {
    bool enable_mutex = iter % 2 == 0;
    auto clock_start = std::chrono::system_clock::now();
    ASSERT_TRUE(BPlusTreeLockBenchmarkCall(32, 10, enable_mutex));
    auto clock_end = std::chrono::system_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(clock_end - clock_start);
    if (enable_mutex) {
      time_ms_with_mutex.push_back(dur.count());
    } else {
      time_ms_wo_mutex.push_back(dur.count());
    }
  }
  std::cout << "This test will see how your B+ tree performance differs with and without contention." << std::endl;
  std::cout << "<<< BEGIN2" << std::endl;
  std::cout << "Normal Access Time: ";
  double ratio_1 = 0;
  double ratio_2 = 0;
  for (auto x : time_ms_wo_mutex) {
    std::cout << x << " ";
    ratio_1 += x;
  }
  std::cout << std::endl;

  std::cout << "Serialized Access Time: ";
  for (auto x : time_ms_with_mutex) {
    std::cout << x << " ";
    ratio_2 += x;
  }
  std::cout << std::endl;
  std::cout << "Ratio: " << ratio_1 / ratio_2 << std::endl;
  std::cout << ">>> END2" << std::endl;
  std::cout << "If your above data is an outlier in all submissions (based on statistics and probably some "
               "machine-learning), TAs will manually inspect your code to ensure you are implementing lock crabbing "
               "correctly."
            << std::endl;
}

}  // namespace bustub
