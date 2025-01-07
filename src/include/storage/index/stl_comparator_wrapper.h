//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// stl_comparator_wrapper.h
//
// Identification: src/include/storage/index/stl_comparator_wrapper.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

namespace bustub {

template <typename KeyType, typename KeyComparator>
class StlComparatorWrapper {
 public:
  explicit StlComparatorWrapper(const KeyComparator &cmp) : cmp_{cmp} {}

  inline auto operator()(const KeyType &lhs, const KeyType &rhs) const -> bool { return cmp_(lhs, rhs) < 0; }

  KeyComparator cmp_;
};
}  // namespace bustub
