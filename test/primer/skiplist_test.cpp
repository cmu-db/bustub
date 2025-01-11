//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// skiplist_test.cpp
//
// Identification: test/primer/skiplist_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/skiplist.h"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <memory>
#include <thread>  // NOLINT
#include <vector>
#include "gtest/gtest.h"

namespace bustub {

/**
 * @brief A instrumented skip list for checking the integrity of the skip list.
 *
 * `InstrumentedSkipList` does not modify the behavior of the `SkipList` baed class, but it provides a `CheckIntegrity`
 * function to check if the skip list is correctly constructed.
 *
 * @tparam K the type of key.
 * @tparam Compare the comparison function for the key.
 * @tparam MaxHeight the maximum height of the skip list.
 * @tparam Seed the seed for the random number generator.
 */
template <typename K, typename Compare = std::less<K>, size_t MaxHeight = 14, uint32_t Seed = 15445>
class InstrumentedSkipList : public SkipList<int, Compare, MaxHeight, Seed> {
 public:
  using Parent = SkipList<int, Compare, MaxHeight, Seed>;

  /**
   * @brief Checks the integrity of the skip list.
   *
   * This method uses GTest internally.
   *
   * @param keys expected keys to be in the skip list, in order.
   * @param heights expected heights of the nodes.
   */
  void CheckIntegrity(const std::vector<int> &keys, const std::vector<uint32_t> &heights) {
    // Check the skip list has the expected number of elements.
    ASSERT_EQ(Parent::Size(), keys.size());

    // Iterate through all the nodes in the skip list.
    size_t pos = 0;
    auto curr = Parent::Header()->Next(0);
    while (curr != nullptr) {
      // Check the key and height of the current node is correct.
      ASSERT_EQ(curr->Height(), heights[pos]);
      ASSERT_EQ(curr->Key(), keys[pos]);

      // Check the link at each level is correct.
      for (size_t level = 0; level < curr->Height(); ++level) {
        for (size_t i = pos + 1; i < keys.size(); ++i) {
          // The first node with a height greater than the current level
          // should be the next node at that level.
          if (heights[i] > level) {
            ASSERT_EQ(curr->Next(level)->Key(), keys[i]);
            break;
          }
        }
      }
      curr = curr->Next(0);
      pos++;
    }

    // Check that we have reached the end of the list.
    ASSERT_EQ(pos, keys.size());
  }
};

TEST(SkipListTest, IntegrityCheckTest) {
  InstrumentedSkipList<int> list;

  // All the keys we will insert into the skip list (1 to 20).
  std::vector<int> keys = {12, 16, 2, 6, 15, 8, 13, 1, 11, 14, 0, 4, 19, 10, 9, 5, 7, 3, 17, 18};

  // Heights of the nodes in the skip list.
  // These will not change since we fixed the seed of the random number generator.
  std::vector<uint32_t> heights = {2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 3, 1, 1, 2, 1, 1, 2, 3};

  // Insert the keys
  for (auto key : keys) {
    list.Insert(key);
  }

  // Sort the keys
  std::sort(keys.begin(), keys.end());

  list.Print();
  // Check that we construct the skip list as expected.
  list.CheckIntegrity(keys, heights);
}

TEST(SkipListTest, InsertContainsTest1) {
  SkipList<int> list;
  ASSERT_EQ(list.Size(), 0);
  ASSERT_TRUE(list.Empty());

  for (int i = 0; i < 10; ++i) {
    ASSERT_TRUE(list.Insert(i));
  }

  for (int i = 0; i < 10; ++i) {
    ASSERT_TRUE(list.Contains(i));
  }

  for (int i = 10; i < 20; ++i) {
    ASSERT_FALSE(list.Contains(i));
  }

  for (int i = 0; i < 10; ++i) {
    ASSERT_FALSE(list.Insert(i));  // Duplicate inserts should fail
  }

  ASSERT_EQ(list.Size(), 10);

  for (int i = 10; i < 20; ++i) {
    ASSERT_TRUE(list.Insert(i));
  }

  ASSERT_EQ(list.Size(), 20);
  ASSERT_FALSE(list.Empty());

  list.Clear();
  ASSERT_EQ(list.Size(), 0);
  ASSERT_TRUE(list.Empty());

  for (int i = 0; i < 30; ++i) {
    ASSERT_FALSE(list.Contains(i));
  }
}

TEST(SkipListTest, InsertContainsTest2) {
  SkipList<int> list;

  // Test insert with an empty list
  ASSERT_EQ(list.Size(), 0);
  ASSERT_TRUE(list.Empty());

  ASSERT_TRUE(list.Insert(1));
  ASSERT_EQ(list.Size(), 1);
  ASSERT_TRUE(list.Insert(2));
  ASSERT_EQ(list.Size(), 2);

  // Test contains for inserted elements
  ASSERT_TRUE(list.Contains(1));
  ASSERT_TRUE(list.Contains(2));

  // Test contains for non-existing element
  ASSERT_FALSE(list.Contains(3));
}

TEST(SkipListTest, InsertAndEraseTest) {
  SkipList<int> list;

  // Insert some elements
  for (int i = 0; i < 5; ++i) {
    ASSERT_TRUE(list.Insert(i));
  }
  ASSERT_EQ(list.Size(), 5);

  // Erase the elements one by one
  for (int i = 0; i < 5; ++i) {
    ASSERT_TRUE(list.Contains(i));
    ASSERT_TRUE(list.Erase(i));
    ASSERT_EQ(list.Size(), 5 - i - 1);  // Size should decrease after each erase
  }

  // After all elements are erased, the list should be empty
  ASSERT_TRUE(list.Empty());
}

TEST(SkipListTest, EraseNonExistingElement) {
  SkipList<int> list;

  // Insert some elements
  for (int i = 0; i < 5; ++i) {
    ASSERT_TRUE(list.Insert(i));
  }

  // Try erasing a non-existing element
  ASSERT_FALSE(list.Erase(10));  // Should return false as element doesn't exist
  ASSERT_EQ(list.Size(), 5);     // Size should remain the same
}

TEST(SkipListTest, ConcurrentInsertTest) {
  SkipList<int> list;
  const int num_threads = 10;
  const int num_insertions_per_thread = 100;
  std::atomic<int> successful_insertions(0);

  auto insert_task = [&list, &successful_insertions](int start) {
    for (int i = start; i < start + num_insertions_per_thread; ++i) {
      bool inserted = list.Insert(i);
      if (inserted) {
        ++successful_insertions;
      }
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(insert_task, i * num_insertions_per_thread);
  }

  for (auto &t : threads) {
    t.join();
  }

  // Check that the correct number of successful insertions occurred
  ASSERT_EQ(successful_insertions.load(), num_threads * num_insertions_per_thread);

  // Verify that all inserted keys are present in the skip list
  for (int i = 0; i < num_threads * num_insertions_per_thread; ++i) {
    ASSERT_TRUE(list.Contains(i));
  }
}

TEST(SkipListTest, ConcurrentEraseTest) {
  SkipList<int> list;

  // Initially insert some elements into the list
  for (int i = 0; i < 100; ++i) {
    list.Insert(i);
  }

  const int num_threads = 10;
  const int num_erasures_per_thread = 10;
  std::atomic<int> successful_erasures(0);

  auto erase_task = [&list, &successful_erasures](int start) {
    for (int i = start; i < start + num_erasures_per_thread; ++i) {
      bool erased = list.Erase(i);
      if (erased) {
        ++successful_erasures;
      }
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(erase_task, i * num_erasures_per_thread);
  }

  for (auto &t : threads) {
    t.join();
  }

  // Verify that the correct number of successful erasures occurred
  ASSERT_EQ(successful_erasures.load(), num_threads * num_erasures_per_thread);

  // Verify that erased elements are no longer in the list
  for (int i = 0; i < 100; ++i) {
    if (i < num_threads * num_erasures_per_thread) {
      ASSERT_FALSE(list.Contains(i));
    } else {
      ASSERT_TRUE(list.Contains(i));
    }
  }
}

TEST(SkipListTest, ConcurrentInsertAndEraseTest) {
  SkipList<int> list;

  // Initially insert some elements into the list
  for (int i = 0; i < 100; ++i) {
    list.Insert(i);
  }

  const int num_threads = 10;
  const int num_operations_per_thread = 10;
  std::atomic<int> successful_insertions(0);
  std::atomic<int> successful_erasures(0);

  auto task = [&list, &successful_insertions, &successful_erasures](int start) {
    for (int i = start; i < start + num_operations_per_thread; ++i) {
      if (!list.Contains(i)) {
        list.Insert(i);
      }

      bool inserted = list.Insert(i + 100);
      if (inserted) {
        ++successful_insertions;
      }

      bool erased = list.Erase(i);
      if (erased) {
        ++successful_erasures;
      }
    }
  };

  std::vector<std::thread> threads;
  threads.reserve(num_threads);
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(task, i * num_operations_per_thread);
  }

  for (auto &t : threads) {
    t.join();
  }

  // Check that the correct number of insertions and erasures occurred
  ASSERT_EQ(successful_insertions.load(), num_threads * num_operations_per_thread);
  ASSERT_EQ(successful_erasures.load(), num_threads * num_operations_per_thread);

  // Verify that elements that were inserted are in the list
  for (int i = 100; i < 100 + num_threads * num_operations_per_thread; ++i) {
    ASSERT_TRUE(list.Contains(i));
  }

  // Verify that elements that were erased are no longer in the list
  for (int i = 0; i < num_threads * num_operations_per_thread; ++i) {
    ASSERT_FALSE(list.Contains(i));
  }
}

TEST(SkipListTest, ConcurrentReadTest) {
  // Number of threads to use for concurrent reads
  const int num_threads = 8;
  std::vector<std::thread> threads;
  threads.reserve(num_threads);

  // Note: you might want to first try use a smaller number of elements
  // if you are running this test on local machine.
  const int total_num_elements = num_threads * 100000;
  auto list = std::make_unique<SkipList<int>>();
  // Insert some elements into the skip list
  for (int i = 0; i < total_num_elements; ++i) {
    list->Insert(i);
  }

  // Function to perform concurrent reads
  auto read_task = [&list](int start, int end) {
    for (int i = start; i < end; ++i) {
      list->Contains(i);
    }
  };

  // Launch threads to perform concurrent reads
  // Note: You will see a timeout if your reads cannot share access to the skip list
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(read_task, 0, total_num_elements);
  }

  // Wait for all threads to complete
  for (auto &t : threads) {
    t.join();
  }
}

}  // namespace bustub
