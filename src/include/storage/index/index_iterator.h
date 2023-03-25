//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/include/index/index_iterator.h
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
/**
 * index_iterator.h
 * For range scan of b+ tree
 */
#pragma once

#include <map>
#include <utility>

#include "storage/index/stl_comparator_wrapper.h"
#include "storage/page/b_plus_tree_leaf_page.h"

namespace bustub {

#define INDEXITERATOR_TYPE IndexIterator<KeyType, ValueType, KeyComparator>

INDEX_TEMPLATE_ARGUMENTS
class IndexIterator {
 public:
  IndexIterator(
      const std::map<KeyType, ValueType, StlComparatorWrapper<KeyType, KeyComparator>> *map,
      typename std::map<KeyType, ValueType, StlComparatorWrapper<KeyType, KeyComparator>>::const_iterator iter)
      : map_(map), iter_(std::move(iter)) {}

  ~IndexIterator() = default;

  auto IsEnd() -> bool { return iter_ != map_->cend(); }

  auto operator*() -> const MappingType & {
    ret_val_ = *iter_;
    return ret_val_;
  }

  auto operator++() -> IndexIterator & {
    iter_++;
    return *this;
  }

  inline auto operator==(const IndexIterator &itr) const -> bool { return itr.iter_ == iter_; }

  inline auto operator!=(const IndexIterator &itr) const -> bool { return !(*this == itr); }

 private:
  const std::map<KeyType, ValueType, StlComparatorWrapper<KeyType, KeyComparator>> *map_;
  typename std::map<KeyType, ValueType, StlComparatorWrapper<KeyType, KeyComparator>>::const_iterator iter_;
  std::pair<KeyType, ValueType> ret_val_;
};

}  // namespace bustub
