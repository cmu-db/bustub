#include <atomic>
#include <chrono>  // NOLINT
#include <iostream>
#include <stdexcept>
#include <thread>  // NOLINT
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "primer/robin_hood_hash_set.h"

namespace bustub {

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

TEST(RobinHoodHashingTest, EdgeTest1) {
  EXPECT_THROW(RobinHoodHashSet<int>(0), std::invalid_argument);

  RobinHoodHashSet<int> table(1);
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(1));
  EXPECT_FALSE(table.Insert(2));
  EXPECT_EQ(table.Size(), 1);
  EXPECT_DOUBLE_EQ(table.LoadFactor(), 1.0);
}

TEST(RobinHoodHashingTest, DuplicateInsertTest) {
  RobinHoodHashSet<int> table(8);
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(9));
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(9));
  EXPECT_EQ(table.Size(), 2);
  EXPECT_TRUE(table.Contains(1));
  EXPECT_TRUE(table.Contains(9));
}

TEST(RobinHoodHashingTest, CollisionAndRobinHoodDisplacementTest) {
  RobinHoodHashSet<int> table(4);

  // std::hash<int> maps these values to the same home bucket on the supported platforms.
  EXPECT_TRUE(table.Insert(0));
  EXPECT_TRUE(table.Insert(4));
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(8));

  // The fourth insert reaches bucket 2 with distance 2 and displaces key 1 (distance 1).
  EXPECT_EQ(table.GetBucket(8), 2);
  EXPECT_EQ(table.GetBucket(1), 3);
  EXPECT_TRUE(table.Contains(0));
  EXPECT_TRUE(table.Contains(4));
  EXPECT_TRUE(table.Contains(1));
  EXPECT_TRUE(table.Contains(8));
  EXPECT_EQ(table.Size(), 4);
}

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

TEST(RobinHoodHashSetTest, MaxProbeDistanceTest) {
  RobinHoodHashSet<int> table(4);
  EXPECT_EQ(table.MaxProbeDistance(), 0);

  EXPECT_TRUE(table.Insert(0));
  EXPECT_TRUE(table.Insert(4));
  EXPECT_TRUE(table.Insert(1));
  EXPECT_TRUE(table.Insert(8));

  // Inserting 8 displaces 1, leaving both 8 and 1 two slots from home.
  EXPECT_EQ(table.MaxProbeDistance(), 2);
  EXPECT_TRUE(table.Remove(8));
  EXPECT_EQ(table.MaxProbeDistance(), 2);
}

TEST(RobinHoodHashingTest, WraparoundTest) {
  RobinHoodHashSet<int> table(8);
  EXPECT_TRUE(table.Insert(6));
  EXPECT_TRUE(table.Insert(14));
  EXPECT_TRUE(table.Insert(22));
  EXPECT_EQ(table.GetBucket(6), 6);
  EXPECT_EQ(table.GetBucket(14), 7);
  EXPECT_EQ(table.GetBucket(22), 0);
  EXPECT_TRUE(table.Contains(22));
}

TEST(RobinHoodHashingTest, TombstoneDeletionTest) {
  RobinHoodHashSet<int> table(8);
  EXPECT_TRUE(table.Insert(6));
  EXPECT_TRUE(table.Insert(14));
  EXPECT_TRUE(table.Insert(22));  // Wraps from buckets 6, 7, then 0.

  EXPECT_EQ(table.GetBucket(22), 0);
  EXPECT_TRUE(table.Remove(14));
  EXPECT_FALSE(table.Contains(14));
  EXPECT_TRUE(table.Contains(22));  // Lookup must continue past the tombstone at bucket 7.
  EXPECT_FALSE(table.Remove(14));
  EXPECT_EQ(table.Size(), 2);
}

TEST(RobinHoodHashingTest, TombstoneReuseTest) {
  RobinHoodHashSet<int> table(8);
  EXPECT_TRUE(table.Insert(6));
  EXPECT_TRUE(table.Insert(14));
  EXPECT_TRUE(table.Insert(22));
  EXPECT_TRUE(table.Remove(14));
  EXPECT_TRUE(table.Insert(30));
  EXPECT_EQ(table.GetBucket(30), 7);
  EXPECT_TRUE(table.Contains(30));
  EXPECT_TRUE(table.Contains(22));
  EXPECT_EQ(table.Size(), 3);
}

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
}

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

TEST(RobinHoodHashingTest, ConcurrentDuplicateInsertTest) {
  RobinHoodHashSet<int> table(64);
  constexpr int num_threads = 16;
  std::atomic<int> ready{0};
  std::atomic<bool> start{false};
  std::atomic<int> successful_inserts{0};
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  for (int i = 0; i < num_threads; i++) {
    threads.emplace_back([&]() {
      ready.fetch_add(1);
      while (!start.load()) {
      }
      if (table.Insert(42)) {
        successful_inserts.fetch_add(1);
      }
    });
  }
  while (ready.load() != num_threads) {
  }
  start.store(true);
  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successful_inserts.load(), num_threads);
  EXPECT_EQ(table.Size(), 1);
  EXPECT_TRUE(table.Contains(42));
}

TEST(RobinHoodHashingTest, ConcurrentInsertAndLookupTest) {
  RobinHoodHashSet<int> table(512);
  constexpr int writer_count = 4;
  constexpr int reader_count = 4;
  constexpr int inserts_per_writer = 64;
  std::atomic<int> ready{0};
  std::atomic<bool> start{false};
  std::atomic<int> successful_inserts{0};
  std::vector<std::thread> threads;
  threads.reserve(writer_count + reader_count);

  for (int writer = 0; writer < writer_count; writer++) {
    threads.emplace_back([&, writer]() {
      ready.fetch_add(1);
      while (!start.load()) {
      }
      for (int i = 0; i < inserts_per_writer; i++) {
        if (table.Insert(writer * inserts_per_writer + i)) {
          successful_inserts.fetch_add(1);
        }
      }
    });
  }
  for (int reader = 0; reader < reader_count; reader++) {
    threads.emplace_back([&, reader]() {
      ready.fetch_add(1);
      while (!start.load()) {
      }
      for (int i = 0; i < writer_count * inserts_per_writer; i++) {
        static_cast<void>(table.Contains((i + reader) % (writer_count * inserts_per_writer)));
      }
    });
  }
  while (ready.load() != writer_count + reader_count) {
  }
  start.store(true);
  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successful_inserts.load(), writer_count * inserts_per_writer);
  EXPECT_EQ(table.Size(), writer_count * inserts_per_writer);
  for (int i = 0; i < writer_count * inserts_per_writer; i++) {
    EXPECT_TRUE(table.Contains(i));
  }
}

TEST(RobinHoodHashingTest, ConcurrentRemoveTest) {
  RobinHoodHashSet<int> table(128);
  constexpr int key_count = 64;
  constexpr int num_threads = 8;
  for (int key = 0; key < key_count; key++) {
    ASSERT_TRUE(table.Insert(key));
  }

  std::atomic<int> ready{0};
  std::atomic<bool> start{false};
  std::atomic<int> successful_removes{0};
  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (int thread_id = 0; thread_id < num_threads; thread_id++) {
    threads.emplace_back([&]() {
      ready.fetch_add(1);
      while (!start.load()) {
      }
      for (int key = 0; key < key_count; key++) {
        if (table.Remove(key)) {
          successful_removes.fetch_add(1);
        }
      }
    });
  }
  while (ready.load() != num_threads) {
  }
  start.store(true);
  for (auto &thread : threads) {
    thread.join();
  }

  EXPECT_EQ(successful_removes.load(), key_count);
  EXPECT_EQ(table.Size(), 0);
  for (int key = 0; key < key_count; key++) {
    EXPECT_FALSE(table.Contains(key));
  }
}

TEST(RobinHoodHashingTest, ConcurrentOverlappingOperationsStressTest) {
  RobinHoodHashSet<int> table(64);
  constexpr int num_threads = 8;
  constexpr int iterations = 500;
  std::atomic<int> ready{0};
  std::atomic<bool> start{false};
  std::atomic<int> completed{0};
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  for (int thread_id = 0; thread_id < num_threads; thread_id++) {
    threads.emplace_back([&, thread_id]() {
      ready.fetch_add(1);
      while (!start.load()) {
      }
      for (int i = 0; i < iterations; i++) {
        const int key = (thread_id * 17 + i) % 96;
        static_cast<void>(table.Insert(key));
        static_cast<void>(table.Contains((key + 1) % 96));
        static_cast<void>(table.Remove((key + 32) % 96));
      }
      completed.fetch_add(1);
    });
  }
  while (ready.load() != num_threads) {
  }
  start.store(true);
  for (auto &thread : threads) {
    thread.join();
  }

  // If an operation deadlocks or livelocks, these joins cannot complete before CTest's timeout.
  EXPECT_EQ(completed.load(), num_threads);
  EXPECT_LE(table.Size(), table.Capacity());
}

TEST(RobinHoodHashingTest, ParallelSpeedupTest) {
  if (std::thread::hardware_concurrency() < 2) {
    GTEST_SKIP() << "Parallel speedup requires at least two hardware threads.";
  }

  constexpr int num_threads = 2;
  constexpr int inserts_per_thread = 100000;
  constexpr int total_inserts = num_threads * inserts_per_thread;
  constexpr int trials = 5;
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
    std::atomic<int> ready{0};
    std::atomic<bool> start{false};
    std::atomic<bool> insert_failed{false};
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int thread_id = 0; thread_id < num_threads; thread_id++) {
      threads.emplace_back([&, thread_id]() {
        ready.fetch_add(1);
        while (!start.load()) {
        }
        const int first_key = thread_id * inserts_per_thread;
        for (int key = first_key; key < first_key + inserts_per_thread; key++) {
          if (!table.Insert(key)) {
            insert_failed.store(true);
          }
        }
      });
    }
    while (ready.load() != num_threads) {
    }
    const auto start_time = std::chrono::steady_clock::now();
    start.store(true);
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
