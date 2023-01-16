#include <fmt/format.h>
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

TEST(TrieStoreTest, BasicTest) {
  auto store = TrieStore();
  ASSERT_EQ(store.Get<uint32_t>("233"), std::nullopt);
  store.Put<uint32_t>("233", 2333);
  {
    auto guard = store.Get<uint32_t>("233");
    ASSERT_EQ(**guard, 2333);
  }
  store.Remove("233");
  {
    auto guard = store.Get<uint32_t>("233");
    ASSERT_EQ(guard, std::nullopt);
  }
}

TEST(TrieStoreTest, GuardTest) {
  auto store = TrieStore();
  ASSERT_EQ(store.Get<uint32_t>("233"), std::nullopt);

  store.Put<std::string>("233", "2333");
  auto guard = store.Get<std::string>("233");
  ASSERT_EQ(**guard, "2333");

  store.Remove("233");
  {
    auto guard = store.Get<std::string>("233");
    ASSERT_EQ(guard, std::nullopt);
  }

  ASSERT_EQ(**guard, "2333");
}

TEST(TrieStoreTest, MixedTest) {
  auto store = TrieStore();
  for (uint32_t i = 0; i < 23333; i++) {
    std::string key = fmt::format("{:#05}", i);
    std::string value = fmt::format("value-{:#08}", i);
    store.Put<std::string>(key, value);
  }
  for (uint32_t i = 0; i < 23333; i += 2) {
    std::string key = fmt::format("{:#05}", i);
    std::string value = fmt::format("new-value-{:#08}", i);
    store.Put<std::string>(key, value);
  }
  for (uint32_t i = 0; i < 23333; i += 3) {
    std::string key = fmt::format("{:#05}", i);
    store.Remove(key);
  }

  // verify final trie
  for (uint32_t i = 0; i < 23333; i++) {
    std::string key = fmt::format("{:#05}", i);
    if (i % 3 == 0) {
      ASSERT_EQ(store.Get<std::string>(key), std::nullopt);
    } else if (i % 2 == 0) {
      std::string value = fmt::format("new-value-{:#08}", i);
      auto guard = store.Get<std::string>(key);
      ASSERT_EQ(**guard, value);
    } else {
      std::string value = fmt::format("value-{:#08}", i);
      auto guard = store.Get<std::string>(key);
      ASSERT_EQ(**guard, value);
    }
  }
}

TEST(TrieStoreTest, MixedConcurrentTest) {
  auto store = TrieStore();

  std::vector<std::thread> threads;

  const int keys_per_thread = 10000;

  for (int tid = 0; tid < 4; tid++) {
    std::thread t([&store, tid] {
      for (uint32_t i = 0; i < keys_per_thread; i++) {
        std::string key = fmt::format("{:#05}", i * 4 + tid);
        std::string value = fmt::format("value-{:#08}", i * 4 + tid);
        store.Put<std::string>(key, value);
      }
      for (uint32_t i = 0; i < keys_per_thread; i++) {
        std::string key = fmt::format("{:#05}", i * 4 + tid);
        store.Remove(key);
      }
      for (uint32_t i = 0; i < keys_per_thread; i++) {
        std::string key = fmt::format("{:#05}", i * 4 + tid);
        std::string value = fmt::format("new-value-{:#08}", i * 4 + tid);
        store.Put<std::string>(key, value);
      }
    });
    threads.push_back(std::move(t));
  }

  for (auto &t : threads) {
    t.join();
  }

  // verify final trie
  for (uint32_t i = 0; i < keys_per_thread * 4; i++) {
    std::string key = fmt::format("{:#05}", i);
    std::string value = fmt::format("new-value-{:#08}", i);
    auto guard = store.Get<std::string>(key);
    ASSERT_EQ(**guard, value);
  }
}

}  // namespace bustub
