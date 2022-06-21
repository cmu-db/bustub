//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// int_comparator.h
//
// Identification: src/include/storage/index/int_comparator.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

namespace bustub {

/**
 * Function object return is > 0 if lhs > rhs, < 0 if lhs < rhs,
 * = 0 if lhs = rhs .
 */
class IntComparator {
 public:
  inline auto operator()(const int lhs, const int rhs) const -> int {
    if (lhs < rhs) {
      return -1;
    }
    if (rhs < lhs) {
      return 1;
    }
    return 0;
  }
};
}  // namespace bustub
