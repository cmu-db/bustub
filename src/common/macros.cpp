#include "common/macros.h"
#include "common/exception.h"
#include "fmt/core.h"

namespace bustub {
void BustubAssertInternal(bool condition, const char *condition_name, const char *file, int line) {
  if (condition) {
    return;
  }
  std::string res = fmt::format("Assertion failed in file {:s} on line {:d}: {:s} ", file, line, condition_name);
  throw InternalException(res);
}

}  // namespace bustub