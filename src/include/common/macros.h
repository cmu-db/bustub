//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// macros.h
//
// Identification: src/include/common/macros.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace bustub {  // NOLINT

#define BUSTUB_ASSERT(expr, message) assert((expr) && (message))

namespace internal {  // NOLINT

// An internal stream, used to take C++ stream-style parameters for display, and exit the program with `std::abort`.
class LogFatalStream {
 public:
  LogFatalStream(const char *file, int line) : file_(file), line_(line) {}

  ~LogFatalStream() {
    std::cerr << file_ << ":" << line_ << ": " << log_stream_.str() << std::endl;
    std::abort();
  }

  template <typename T>
  auto operator<<(const T &val) -> LogFatalStream & {
    log_stream_ << val;
    return *this;
  }

 private:
  const char *file_;
  int line_;
  std::ostringstream log_stream_;
};

}  // namespace internal

// A macro which checks `expr` value and performs assert.
// Different from `BUSTUB_ASSERT`, it takes stream-style parameters.
#define BUSTUB_ASSERT_AND_LOG(expr)                       /*NOLINT*/ \
  if (bool val = (expr); !val) internal::LogFatalStream { /*NOLINT*/ \
      __FILE__, __LINE__                                  /*NOLINT*/ \
    }

#define UNIMPLEMENTED(message) throw std::logic_error(message)

#define BUSTUB_ENSURE(expr, message)                  \
  if (!(expr)) {                                      \
    std::cerr << "ERROR: " << (message) << std::endl; \
    std::terminate();                                 \
  }

#define UNREACHABLE(message) throw std::logic_error(message)

// Macros to disable copying and moving
#define DISALLOW_COPY(cname)                                    \
  cname(const cname &) = delete;                   /* NOLINT */ \
  auto operator=(const cname &)->cname & = delete; /* NOLINT */

#define DISALLOW_MOVE(cname)                               \
  cname(cname &&) = delete;                   /* NOLINT */ \
  auto operator=(cname &&)->cname & = delete; /* NOLINT */

#define DISALLOW_COPY_AND_MOVE(cname) \
  DISALLOW_COPY(cname);               \
  DISALLOW_MOVE(cname);

}  // namespace bustub
