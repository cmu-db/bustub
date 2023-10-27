//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_hash_table_index.h
//
// Identification: src/include/storage/index/extendible_hash_table_index.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "container/disk/hash/disk_extendible_hash_table.h"
#include "container/hash/hash_function.h"
#include "storage/index/index.h"

namespace bustub {

#define HASH_TABLE_INDEX_TYPE ExtendibleHashTableIndex<KeyType, ValueType, KeyComparator>

template <typename KeyType, typename ValueType, typename KeyComparator>
class ExtendibleHashTableIndex : public Index {
 public:
  ExtendibleHashTableIndex(std::unique_ptr<IndexMetadata> &&metadata, BufferPoolManager *buffer_pool_manager,
                           const HashFunction<KeyType> &hash_fn);

  ~ExtendibleHashTableIndex() override = default;

  auto InsertEntry(const Tuple &key, RID rid, Transaction *transaction) -> bool override;

  void DeleteEntry(const Tuple &key, RID rid, Transaction *transaction) override;

  void ScanKey(const Tuple &key, std::vector<RID> *result, Transaction *transaction) override;

 protected:
  // comparator for key
  KeyComparator comparator_;
  // container
  DiskExtendibleHashTable<KeyType, ValueType, KeyComparator> container_;
};

constexpr static const auto TWO_INTEGER_SIZE = 8;
using IntegerKeyType = GenericKey<TWO_INTEGER_SIZE>;
using IntegerValueType = RID;
using IntegerComparatorType = GenericComparator<TWO_INTEGER_SIZE>;
using HashTableIndexForTwoIntegerColumn =
    ExtendibleHashTableIndex<IntegerKeyType, IntegerValueType, IntegerComparatorType>;

}  // namespace bustub
