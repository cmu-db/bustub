//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree_index.h
//
// Identification: src/include/storage/index/b_plus_tree_index.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "container/hash/hash_function.h"
#include "storage/index/b_plus_tree.h"
#include "storage/index/index.h"

namespace bustub {

#define BPLUSTREE_INDEX_TYPE BPlusTreeIndex<KeyType, ValueType, KeyComparator>

INDEX_TEMPLATE_ARGUMENTS
class BPlusTreeIndex : public Index {
 public:
  BPlusTreeIndex(std::unique_ptr<IndexMetadata> &&metadata, BufferPoolManager *buffer_pool_manager);

  auto InsertEntry(const Tuple &key, RID rid, Transaction *transaction) -> bool override;

  void DeleteEntry(const Tuple &key, RID rid, Transaction *transaction) override;

  void ScanKey(const Tuple &key, std::vector<RID> *result, Transaction *transaction) override;

  auto GetBeginIterator() -> INDEXITERATOR_TYPE;

  auto GetBeginIterator(const KeyType &key) -> INDEXITERATOR_TYPE;

  auto GetEndIterator() -> INDEXITERATOR_TYPE;

 protected:
  // comparator for key
  KeyComparator comparator_;
  // container
  std::shared_ptr<BPlusTree<KeyType, ValueType, KeyComparator>> container_;
};

/** We only support index table with one integer key for now in BusTub. Hardcode everything here. */

constexpr static const auto TWO_INTEGER_SIZE_B_TREE = 8;
using IntegerKeyType_BTree = GenericKey<TWO_INTEGER_SIZE_B_TREE>;
using IntegerValueType_BTree = RID;
using IntegerComparatorType_BTree = GenericComparator<TWO_INTEGER_SIZE_B_TREE>;
using BPlusTreeIndexForTwoIntegerColumn =
    BPlusTreeIndex<IntegerKeyType_BTree, IntegerValueType_BTree, IntegerComparatorType_BTree>;
using BPlusTreeIndexIteratorForTwoIntegerColumn =
    IndexIterator<IntegerKeyType_BTree, IntegerValueType_BTree, IntegerComparatorType_BTree>;
using IntegerHashFunctionType = HashFunction<IntegerKeyType_BTree>;

}  // namespace bustub
