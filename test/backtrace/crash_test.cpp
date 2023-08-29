#include "common/exception.h"
#include "common/macros.h"
#include "gtest/gtest.h"

// NOLINTBEGIN

namespace bustub {

TEST(CrashTest, DISABLED_PtrAccess) {
  // ASAN will show the full backtrace
  int *p = nullptr;
  *p = 2;
}

TEST(CrashTest, DISABLED_GtestAssert) {
  // Gtest will show the line that failed
  ASSERT_TRUE(false);
}

TEST(CrashTest, DISABLED_Assert) {
  // Default assertion implementation, no backtrace, only lineno
  BUSTUB_ASSERT(false, "assert failure");
}

TEST(CrashTest, DISABLED_Ensure) {
  // Full stacktrace provided by backward-cpp
  BUSTUB_ENSURE(false, "assert failure");
}

TEST(CrashTest, Throw) {}

}  // namespace bustub

// NOLINTEND
