//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// linear_probe_hash_table_index.h
//
// Identification: src/include/storage/index/linear_probe_hash_table_index.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "container/disk/hash/linear_probe_hash_table.h"
#include "container/hash/hash_function.h"
#include "storage/index/index.h"

namespace bustub {

#define HASH_TABLE_INDEX_TYPE LinearProbeHashTableIndex<KeyType, ValueType, KeyComparator>

template <typename KeyType, typename ValueType, typename KeyComparator>
class LinearProbeHashTableIndex : public Index {
 public:
  LinearProbeHashTableIndex(std::unique_ptr<IndexMetadata> &&metadata, BufferPoolManager *buffer_pool_manager,
                            size_t num_buckets, const HashFunction<KeyType> &hash_fn);

  ~LinearProbeHashTableIndex() override = default;

  auto InsertEntry(const Tuple &key, RID rid, Transaction *transaction) -> bool override;

  void DeleteEntry(const Tuple &key, RID rid, Transaction *transaction) override;

  void ScanKey(const Tuple &key, std::vector<RID> *result, Transaction *transaction) override;

 protected:
  // comparator for key
  KeyComparator comparator_;
  // container
  LinearProbeHashTable<KeyType, ValueType, KeyComparator> container_;
};

}  // namespace bustub
