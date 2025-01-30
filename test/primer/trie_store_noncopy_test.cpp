//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// trie_store_noncopy_test.cpp
//
// Identification: test/primer/trie_store_noncopy_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <bitset>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <random>
#include <thread>  // NOLINT

#include "common/exception.h"
#include "gtest/gtest.h"
#include "primer/trie.h"
#include "primer/trie_store.h"

namespace bustub {

using Integer = std::unique_ptr<uint32_t>;

TEST(TrieStoreTest, NonCopyableTest) {
  auto store = TrieStore();
  store.Put<Integer>("tes", std::make_unique<uint32_t>(233));
  store.Put<Integer>("te", std::make_unique<uint32_t>(23));
  store.Put<Integer>("test", std::make_unique<uint32_t>(2333));
  ASSERT_EQ(***store.Get<Integer>("te"), 23);
  ASSERT_EQ(***store.Get<Integer>("tes"), 233);
  ASSERT_EQ(***store.Get<Integer>("test"), 2333);
  store.Remove("te");
  store.Remove("tes");
  store.Remove("test");
  ASSERT_EQ(store.Get<Integer>("te"), std::nullopt);
  ASSERT_EQ(store.Get<Integer>("tes"), std::nullopt);
  ASSERT_EQ(store.Get<Integer>("test"), std::nullopt);
}

TEST(TrieStoreTest, ReadWriteTest) {
  auto store = TrieStore();
  store.Put<uint32_t>("a", 1);
  store.Put<uint32_t>("b", 2);
  store.Put<uint32_t>("c", 3);
  std::promise<int> x;

  std::cerr << "[0] begin" << std::endl;

  std::thread t([&store, &x] { store.Put<MoveBlocked>("d", MoveBlocked(x.get_future())); });

  std::cerr << "[1] thread spawn" << std::endl;

  // Loop for enough time to ensure that the thread is blocked on the promise.
  for (int i = 0; i < 100000; i++) {
    {
      auto guard = store.Get<uint32_t>("a");
      ASSERT_EQ(**guard, 1);
    }
    {
      auto guard = store.Get<uint32_t>("b");
      ASSERT_EQ(**guard, 2);
    }
    {
      auto guard = store.Get<uint32_t>("c");
      ASSERT_EQ(**guard, 3);
    }
  }

  std::cerr << "[2] read done" << std::endl;

  x.set_value(233);

  t.join();

  std::cerr << "[3] write complete" << std::endl;

  ASSERT_NE(store.Get<MoveBlocked>("d"), std::nullopt);
}

}  // namespace bustub
