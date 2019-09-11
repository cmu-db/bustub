/**
 * clock_replacer_test.cpp
 */

#include <cstdio>
#include <vector>
#include <thread>

#include "buffer/clock_replacer.h"
#include "gtest/gtest.h"

namespace bustub {

TEST(ClockReplacerTest, DISABLED_SampleTest) {
  ClockReplacer clock_replacer(7);
  
  // push element into replacer
  clock_replacer.Unpin(1);
  clock_replacer.Unpin(2);
  clock_replacer.Unpin(3);
  clock_replacer.Unpin(4);
  clock_replacer.Unpin(5);
  clock_replacer.Unpin(6);
  clock_replacer.Unpin(1);
  EXPECT_EQ(6, clock_replacer.Size());
  
  // pop element from replacer
  int value;
  clock_replacer.Victim(&value);
  EXPECT_EQ(1, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(2, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(3, value);
  
  // Pin elements in replacer, 3 has already evicted so has no effect.
  clock_replacer.Pin(3);
  clock_replacer.Pin(4);
  EXPECT_EQ(2, clock_replacer.Size());
  // Unpin 4, its reference bit should be set to 1
  clock_replacer.Unpin(4);

  // pop element from replacer after removal
  clock_replacer.Victim(&value);
  EXPECT_EQ(5, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(6, value);
  clock_replacer.Victim(&value);
  EXPECT_EQ(4, value);
}

} // namespace bustub