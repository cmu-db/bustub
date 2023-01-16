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

TEST(TrieTest, NonCopyableTest) {
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

}  // namespace bustub
