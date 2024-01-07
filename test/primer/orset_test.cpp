#include "primer/orset.h"
#include "common/exception.h"
#include "fmt/core.h"
#include "fmt/format.h"
#include "fmt/ranges.h"
#include "gtest/gtest.h"

namespace bustub {

TEST(ORSetTest, SimpleAddRemoveTest) {
  const int n = 10;
  auto orset = ORSet<int>();

  fmt::println("before insertions: {{{}}}", fmt::join(orset.Elements(), ", "));

  for (int i = 0; i < n; ++i) {
    orset.Add(i);
    ASSERT_TRUE(orset.Contains(i));
  }

  fmt::println("after insertions: {{{}}}", fmt::join(orset.Elements(), ", "));

  for (int i = 0; i < n; ++i) {
    ASSERT_TRUE(orset.Contains(i));
    orset.Remove(i);
    ASSERT_FALSE(orset.Contains(i));
  }

  fmt::println("after removals: {{{}}}", fmt::join(orset.Elements(), ", "));
}

TEST(ORSetTest, SimpleMergeTest) {
  auto orset_a = ORSet<int>();
  auto orset_b = ORSet<int>();
  orset_a.Add(1);
  orset_b.Add(2);
  orset_b.Remove(2);
  orset_a.Merge(orset_b);

  ASSERT_TRUE(orset_a.Contains(1));
  ASSERT_FALSE(orset_a.Contains(2));
  ASSERT_FALSE(orset_b.Contains(1));
  ASSERT_FALSE(orset_b.Contains(2));

  orset_b.Merge(orset_a);
  ASSERT_TRUE(orset_b.Contains(1));

  orset_b.Add(2);
  ASSERT_TRUE(orset_b.Contains(2));

  fmt::println("orset_a: {{{}}}", fmt::join(orset_a.Elements(), ", "));
  fmt::println("orset_b: {{{}}}", fmt::join(orset_b.Elements(), ", "));
}

}  // namespace bustub
