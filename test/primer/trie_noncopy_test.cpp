//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// trie_noncopy_test.cpp
//
// Identification: test/primer/trie_noncopy_test.cpp
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

namespace bustub {

using Integer = std::unique_ptr<uint32_t>;

TEST(TrieTest, NonCopyableTest) {
  auto trie = Trie();
  trie = trie.Put<Integer>("tes", std::make_unique<uint32_t>(233));
  trie = trie.Put<Integer>("te", std::make_unique<uint32_t>(23));
  trie = trie.Put<Integer>("test", std::make_unique<uint32_t>(2333));
  ASSERT_EQ(**trie.Get<Integer>("te"), 23);
  ASSERT_EQ(**trie.Get<Integer>("tes"), 233);
  ASSERT_EQ(**trie.Get<Integer>("test"), 2333);
  trie = trie.Remove("te");
  trie = trie.Remove("tes");
  trie = trie.Remove("test");
  ASSERT_EQ(trie.Get<Integer>("te"), nullptr);
  ASSERT_EQ(trie.Get<Integer>("tes"), nullptr);
  ASSERT_EQ(trie.Get<Integer>("test"), nullptr);
}

}  // namespace bustub
