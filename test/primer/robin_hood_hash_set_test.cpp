#include <atomic>
#include <chrono>              // NOLINT
#include <condition_variable>  // NOLINT
#include <iostream>
#include <mutex>  // NOLINT
#include <stdexcept>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "primer/robin_hood_hash_set.h"

namespace bustub {

class ThreadGate {
 public:
  explicit ThreadGate(size_t thread_count) : thread_count_(thread_count) {}

  void ArriveAndWait() {
    std::unique_lock lock(mutex_);
    ready_++;
    if (ready_ == thread_count_) {
      ready_cv_.notify_one();
    }
    start_cv_.wait(lock, [this] { return started_; });
  }

  void WaitUntilReady() {
    std::unique_lock lock(mutex_);
    ready_cv_.wait(lock, [this] { return ready_ == thread_count_; });
  }

  void Open() {
    {
      std::lock_guard lock(mutex_);
      started_ = true;
    }
    start_cv_.notify_all();
  }

 private:
  const size_t thread_count_;
  size_t ready_{0};
  bool started_{false};
  std::mutex mutex_;
  std::condition_variable ready_cv_;
  std::condition_variable start_cv_;
};

auto FindKeysWithHomeBucket(size_t capacity, size_t home_bucket, size_t count) -> std::vector<int> {
  std::vector<int> keys;
  for (int key = 0; key < 10000 && keys.size() < count; key++) {
    if (RobinHoodHash<int>{}(key) % capacity == home_bucket) {
      keys.push_back(key);
    }
  }
  if (keys.size() != count) {
    throw std::runtime_error("Could not find enough colliding keys for the test.");
  }
  return keys;
}

// Verifies basic insertion, duplicate replacement, lookup, and table statistics.
TEST(RobinHoodHashingTest, BasicTest1) {
  RobinHoodHashSet<int> table(8);
  EXPECT_EQ(table.Capacity(), 8);
  EXPECT_EQ(table.BucketCount(), 8);
  EXPECT_EQ(table.Size(), 0);
  EXPECT_DOUBLE_EQ(table.LoadFactor(), 0.0);
  EXPECT_FALSE(table.Contains(1));

  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(2));
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Contains(1));
  EXPECT_TRUE(table.Contains(2));
  EXPECT_EQ(table.Size(), 2);
  EXPECT_DOUBLE_EQ(table.LoadFactor(), 0.25);
}

// Verifies that the set supports the string key type.
TEST(RobinHoodHashingTest, BasicTest2) {
  RobinHoodHashSet<std::string> table(7);
  EXPECT_TRUE(table.Insert("BusTub"));
  EXPECT_TRUE(table.Insert("CMU DB"));
  EXPECT_TRUE(table.Insert("15-445"));
  EXPECT_TRUE(table.Contains("BusTub"));
  EXPECT_TRUE(table.Contains("CMU DB"));
  EXPECT_TRUE(table.Contains("15-445"));
  EXPECT_FALSE(table.Contains("missing"));
  EXPECT_EQ(table.Size(), 3);
}

// Verifies constructor validation, single-bucket behavior, and full-table rejection.
TEST(RobinHoodHashingTest, EdgeTest1) {
  EXPECT_THROW(RobinHoodHashSet<int>(0), std::invalid_argument);

  RobinHoodHashSet<int> table(1);
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(1));
  EXPECT_FALSE(table.Insert(2));
  EXPECT_EQ(table.Size(), 1);
  EXPECT_DOUBLE_EQ(table.LoadFactor(), 1.0);
}

// Verifies Robin Hood displacement when an incoming key has a greater probe distance.
TEST(RobinHoodHashingTest, CollisionAndRobinHoodDisplacementTest) {
  RobinHoodHashSet<int> table(4);
  const auto colliding_keys = FindKeysWithHomeBucket(4, 0, 3);
  const auto next_bucket_key = FindKeysWithHomeBucket(4, 1, 1).front();

  EXPECT_TRUE(table.Insert(colliding_keys[0]));
  EXPECT_TRUE(table.Insert(colliding_keys[1]));
  EXPECT_TRUE(table.Insert(next_bucket_key));
  EXPECT_TRUE(table.Insert(colliding_keys[2]));

  // The fourth insert displaces the key whose home bucket is 1.
  EXPECT_EQ(table.GetBucket(colliding_keys[2]), 2);
  EXPECT_EQ(table.GetBucket(next_bucket_key), 3);
  EXPECT_EQ(table.GetBucket(99), table.BucketCount());
  EXPECT_TRUE(table.Contains(colliding_keys[0]));
  EXPECT_TRUE(table.Contains(colliding_keys[1]));
  EXPECT_TRUE(table.Contains(next_bucket_key));
  EXPECT_TRUE(table.Contains(colliding_keys[2]));
  EXPECT_EQ(table.Size(), 4);
}

// Verifies that insertion reports failure only after every bucket is occupied.
TEST(RobinHoodHashingTest, FullTableTest) {
  RobinHoodHashSet<int> table(4);
  EXPECT_TRUE(table.Insert(0));
  EXPECT_TRUE(table.Insert(4));
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(8));
  EXPECT_FALSE(table.Insert(12));
  EXPECT_EQ(table.Size(), 4);
  EXPECT_TRUE(table.Contains(0));
  EXPECT_TRUE(table.Contains(4));
  EXPECT_TRUE(table.Contains(1));
  EXPECT_TRUE(table.Contains(8));
}

// Verifies the maximum probe distance among live entries after a tombstone is created.
TEST(RobinHoodHashSetTest, MaxProbeDistanceTest) {
  RobinHoodHashSet<int> table(4);
  const auto colliding_keys = FindKeysWithHomeBucket(4, 0, 3);
  EXPECT_EQ(table.MaxProbeDistance(), 0);

  EXPECT_TRUE(table.Insert(colliding_keys[0]));
  EXPECT_TRUE(table.Insert(colliding_keys[1]));
  EXPECT_TRUE(table.Insert(colliding_keys[2]));

  // The final colliding key lands two positions after its home bucket.
  EXPECT_EQ(table.GetBucket(colliding_keys[0]), 0);
  EXPECT_EQ(table.GetBucket(colliding_keys[1]), 1);
  EXPECT_EQ(table.GetBucket(colliding_keys[2]), 2);
  EXPECT_EQ(table.MaxProbeDistance(), 2);
  EXPECT_TRUE(table.Remove(colliding_keys[2]));
  EXPECT_EQ(table.MaxProbeDistance(), 1);
}

// Verifies linear probing wraps from the final bucket back to bucket zero.
TEST(RobinHoodHashingTest, WraparoundTest) {
  RobinHoodHashSet<int> table(8);
  const auto colliding_keys = FindKeysWithHomeBucket(8, 6, 3);
  EXPECT_TRUE(table.Insert(colliding_keys[0]));
  EXPECT_TRUE(table.Insert(colliding_keys[1]));
  EXPECT_TRUE(table.Insert(colliding_keys[2]));
  EXPECT_EQ(table.GetBucket(colliding_keys[0]), 6);
  EXPECT_EQ(table.GetBucket(colliding_keys[1]), 7);
  EXPECT_EQ(table.GetBucket(colliding_keys[2]), 0);
  EXPECT_TRUE(table.Contains(colliding_keys[2]));
}

// Verifies that lookups continue through a tombstone created inside a probe chain.
TEST(RobinHoodHashingTest, TombstoneDeletionTest) {
  RobinHoodHashSet<int> table(8);
  const auto colliding_keys = FindKeysWithHomeBucket(8, 6, 3);
  EXPECT_TRUE(table.Insert(colliding_keys[0]));
  EXPECT_TRUE(table.Insert(colliding_keys[1]));
  EXPECT_TRUE(table.Insert(colliding_keys[2]));  // Wraps from buckets 6, 7, then 0.

  EXPECT_EQ(table.GetBucket(colliding_keys[2]), 0);
  EXPECT_TRUE(table.Remove(colliding_keys[1]));
  EXPECT_FALSE(table.Contains(colliding_keys[1]));
  EXPECT_TRUE(table.Contains(colliding_keys[2]));  // Lookup must continue past the tombstone at bucket 7.
  EXPECT_FALSE(table.Remove(colliding_keys[1]));
  EXPECT_EQ(table.Size(), 2);
}

// Verifies that insertion reuses a tombstone without breaking later probe-chain lookups.
TEST(RobinHoodHashingTest, TombstoneReuseTest) {
  RobinHoodHashSet<int> table(8);
  const auto colliding_keys = FindKeysWithHomeBucket(8, 6, 4);
  EXPECT_TRUE(table.Insert(colliding_keys[0]));
  EXPECT_TRUE(table.Insert(colliding_keys[1]));
  EXPECT_TRUE(table.Insert(colliding_keys[2]));
  EXPECT_TRUE(table.Remove(colliding_keys[1]));
  EXPECT_TRUE(table.Insert(colliding_keys[3]));
  EXPECT_EQ(table.GetBucket(colliding_keys[3]), 7);
  EXPECT_TRUE(table.Contains(colliding_keys[3]));
  EXPECT_TRUE(table.Contains(colliding_keys[2]));
  EXPECT_EQ(table.Size(), 3);
}

// Verifies move construction and assignment transfer contents and leave sources usable.
TEST(RobinHoodHashingTest, MoveTest) {
  RobinHoodHashSet<int> source(8);
  EXPECT_TRUE(source.Insert(1));
  EXPECT_TRUE(source.Insert(9));

  RobinHoodHashSet<int> moved(std::move(source));
  EXPECT_EQ(moved.Capacity(), 8);
  EXPECT_EQ(moved.Size(), 2);
  EXPECT_TRUE(moved.Contains(1));
  EXPECT_TRUE(moved.Contains(9));
  EXPECT_EQ(source.Capacity(), 0);  // NOLINT
  EXPECT_EQ(source.Size(), 0);

  RobinHoodHashSet<int> assigned(4);
  EXPECT_TRUE(assigned.Insert(42));
  assigned = std::move(moved);
  EXPECT_EQ(assigned.Capacity(), 8);
  EXPECT_EQ(assigned.Size(), 2);
  EXPECT_TRUE(assigned.Contains(1));
  EXPECT_TRUE(assigned.Contains(9));
  EXPECT_FALSE(assigned.Contains(42));

  EXPECT_TRUE(assigned.Insert(17));
  EXPECT_TRUE(assigned.Contains(17));
  EXPECT_EQ(assigned.Size(), 3);

  EXPECT_TRUE(assigned.Remove(1));
  EXPECT_FALSE(assigned.Contains(1));
  EXPECT_TRUE(assigned.Contains(9));
  EXPECT_TRUE(assigned.Contains(17));
  EXPECT_EQ(assigned.Size(), 2);
}

// Verifies clearing live entries and tombstones, including moved-from and assigned tables.
TEST(RobinHoodHashingTest, ClearTest) {
  RobinHoodHashSet<int> table(8);
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(9));
  EXPECT_TRUE(table.Remove(1));

  table.Clear();
  EXPECT_EQ(table.Capacity(), 8);
  EXPECT_EQ(table.Size(), 0);
  EXPECT_DOUBLE_EQ(table.LoadFactor(), 0.0);
  EXPECT_FALSE(table.Contains(1));
  EXPECT_FALSE(table.Contains(9));

  EXPECT_TRUE(table.Insert(17));
  EXPECT_TRUE(table.Contains(17));

  RobinHoodHashSet<int> moved_from(4);
  RobinHoodHashSet<int> moved(std::move(moved_from));
  moved_from.Clear();  // NOLINT
  EXPECT_EQ(moved_from.Size(), 0);
  EXPECT_EQ(moved_from.Capacity(), 0);
  moved.Clear();
  EXPECT_EQ(moved.Size(), 0);
  EXPECT_EQ(moved.Capacity(), 4);

  RobinHoodHashSet<int> assigned(4);
  assigned = std::move(moved);
  assigned.Clear();
  EXPECT_EQ(assigned.Size(), 0);
  EXPECT_DOUBLE_EQ(assigned.LoadFactor(), 0.0);
}

// Verifies concurrent duplicate inserts preserve one logical set entry.
TEST(RobinHoodHashingTest, ConcurrentDuplicateInsertTest) {
  RobinHoodHashSet<int> table(64);
  constexpr int num_threads = 16;
  ThreadGate gate(num_threads);
  std::atomic<int> successful_inserts{0};
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back([&]() {
      gate.ArriveAndWait();
      if (table.Insert(42)) {
        successful_inserts.fetch_add(1);
      }
    });
  }
  gate.WaitUntilReady();
  gate.Open();
  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successful_inserts.load(), num_threads);
  EXPECT_EQ(table.Size(), 1);
  EXPECT_TRUE(table.Contains(42));
}

// Verifies concurrent writers and readers complete without losing inserted keys.
TEST(RobinHoodHashingTest, ConcurrentInsertAndLookupTest) {
  RobinHoodHashSet<int> table(512);
  constexpr int writer_count = 4;
  constexpr int reader_count = 4;
  constexpr int inserts_per_writer = 64;
  ThreadGate gate(writer_count + reader_count);
  std::atomic<int> successful_inserts{0};
  std::vector<std::thread> threads;
  threads.reserve(writer_count + reader_count);

  for (int writer = 0; writer < writer_count; writer++) {
    threads.emplace_back([&, writer]() {
      gate.ArriveAndWait();
      for (int i = 0; i < inserts_per_writer; i++) {
        if (table.Insert(writer * inserts_per_writer + i)) {
          successful_inserts.fetch_add(1);
        }
      }
    });
  }
  for (int reader = 0; reader < reader_count; reader++) {
    threads.emplace_back([&, reader]() {
      gate.ArriveAndWait();
      for (int i = 0; i < writer_count * inserts_per_writer; i++) {
        // Concurrent writers make each individual lookup result nondeterministic.
        static_cast<void>(table.Contains((i + reader) % (writer_count * inserts_per_writer)));
      }
    });
  }
  gate.WaitUntilReady();
  gate.Open();
  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successful_inserts.load(), writer_count * inserts_per_writer);
  EXPECT_EQ(table.Size(), writer_count * inserts_per_writer);
  for (int i = 0; i < writer_count * inserts_per_writer; i++) {
    EXPECT_TRUE(table.Contains(i));
  }
}

// Verifies concurrent removes report exactly one successful removal per key.
TEST(RobinHoodHashingTest, ConcurrentRemoveTest) {
  RobinHoodHashSet<int> table(128);
  constexpr int key_count = 64;
  constexpr int num_threads = 8;
  for (int key = 0; key < key_count; key++) {
    ASSERT_TRUE(table.Insert(key));
  }

  ThreadGate gate(num_threads);
  std::atomic<int> successful_removes{0};
  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (int thread_id = 0; thread_id < num_threads; thread_id++) {
    threads.emplace_back([&]() {
      gate.ArriveAndWait();
      for (int key = 0; key < key_count; key++) {
        if (table.Remove(key)) {
          successful_removes.fetch_add(1);
        }
      }
    });
  }
  gate.WaitUntilReady();
  gate.Open();
  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successful_removes.load(), key_count);
  EXPECT_EQ(table.Size(), 0);
  for (int key = 0; key < key_count; key++) {
    EXPECT_FALSE(table.Contains(key));
  }
}

// Verifies overlapping inserts, lookups, and removes make progress without corrupting metadata.
TEST(RobinHoodHashingTest, ConcurrentOverlappingOperationsStressTest) {
  RobinHoodHashSet<int> table(64);
  constexpr int num_threads = 8;
  constexpr int iterations = 500;
  ThreadGate gate(num_threads);
  std::atomic<int> completed{0};
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  for (int thread_id = 0; thread_id < num_threads; thread_id++) {
    threads.emplace_back([&, thread_id]() {
      gate.ArriveAndWait();
      for (int i = 0; i < iterations; i++) {
        const int key = (thread_id * 17 + i) % 96;
        // Overlapping operations make individual return values nondeterministic.
        static_cast<void>(table.Insert(key));
        static_cast<void>(table.Contains((key + 1) % 96));
        static_cast<void>(table.Remove((key + 32) % 96));
      }
      completed.fetch_add(1);
    });
  }
  gate.WaitUntilReady();
  gate.Open();
  for (auto &thread : threads) {
    thread.join();
  }

  // If an operation deadlocks or livelocks, these joins cannot complete before CTest's timeout.
  EXPECT_EQ(completed.load(), num_threads);
  EXPECT_LE(table.Size(), table.Capacity());
}

// Verifies independent insert workloads benefit from the table's striped synchronization.
TEST(RobinHoodHashingTest, ParallelSpeedupTest) {
  if (std::thread::hardware_concurrency() < 2) {
    GTEST_SKIP() << "Parallel speedup requires at least two hardware threads.";
  }

  constexpr int num_threads = 2;
  constexpr int inserts_per_thread = 100000;
  constexpr int total_inserts = num_threads * inserts_per_thread;
  constexpr int trials = 5;
  // Keep the table below its fixed capacity so each trial measures insertion rather than saturation.
  constexpr size_t capacity = 262144;
  int64_t sequential_time_us = 0;
  int64_t parallel_time_us = 0;

  for (int trial = 0; trial < trials; trial++) {
    RobinHoodHashSet<int> table(capacity);
    const auto start_time = std::chrono::steady_clock::now();
    for (int key = 0; key < total_inserts; key++) {
      ASSERT_TRUE(table.Insert(key));
    }
    const auto end_time = std::chrono::steady_clock::now();
    sequential_time_us += std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    ASSERT_EQ(table.Size(), total_inserts);
  }

  for (int trial = 0; trial < trials; trial++) {
    RobinHoodHashSet<int> table(capacity);
    ThreadGate gate(num_threads);
    std::atomic<bool> insert_failed{false};
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int thread_id = 0; thread_id < num_threads; thread_id++) {
      threads.emplace_back([&, thread_id]() {
        gate.ArriveAndWait();
        const int first_key = thread_id * inserts_per_thread;
        for (int key = first_key; key < first_key + inserts_per_thread; key++) {
          if (!table.Insert(key)) {
            insert_failed.store(true);
          }
        }
      });
    }
    gate.WaitUntilReady();
    const auto start_time = std::chrono::steady_clock::now();
    gate.Open();
    for (auto &thread : threads) {
      thread.join();
    }
    const auto end_time = std::chrono::steady_clock::now();
    parallel_time_us += std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    ASSERT_FALSE(insert_failed.load());
    ASSERT_EQ(table.Size(), total_inserts);
  }

  ASSERT_GT(parallel_time_us, 0);
  const double speedup = static_cast<double>(sequential_time_us) / static_cast<double>(parallel_time_us);
  std::cout << "Sequential insertion time: " << sequential_time_us << " us\n"
            << "Parallel insertion time: " << parallel_time_us << " us\n"
            << "Speedup: " << speedup << 'x' << std::endl;
  EXPECT_GT(speedup, 1.2) << "Disjoint concurrent inserts should outperform the same sequential workload.";
}

}  // namespace bustub
