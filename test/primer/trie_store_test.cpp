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

TEST(TrieStoreTest, BasicTest) {
  auto store = TrieStore();
  store.Get<uint32_t>("233");
  store.Put<uint32_t>("233", 2333);
}

}  // namespace bustub
