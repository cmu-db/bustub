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

namespace bustub {

/**
 * Function object return is > 0 if lhs > rhs, < 0 if lhs < rhs,
 * = 0 if lhs = rhs .
 */
class IntComparator {
 public:
  inline int operator()(const int lhs, const int rhs) { return lhs - rhs; }
};
}  // namespace bustub
