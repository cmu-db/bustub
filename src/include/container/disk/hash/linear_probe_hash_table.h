//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// linear_probe_hash_table.h
//
// Identification: src/include/container/disk/hash/linear_probe_hash_table.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <queue>
#include <string>
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "concurrency/transaction.h"
#include "container/hash/hash_function.h"
#include "storage/page/hash_table_block_page.h"
#include "storage/page/hash_table_header_page.h"

namespace bustub {

#define HASH_TABLE_TYPE LinearProbeHashTable<KeyType, ValueType, KeyComparator>

/**
 * Implementation of linear probing hash table that is backed by a buffer pool
 * manager. Non-unique keys are supported. Supports insert and delete. The
 * table dynamically grows once full.
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
class LinearProbeHashTable {
 public:
  explicit LinearProbeHashTable(const std::string &name, BufferPoolManager *buffer_pool_manager,
                                const KeyComparator &comparator, size_t num_buckets, HashFunction<KeyType> hash_fn);

  auto Insert(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool;

  auto Remove(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool;

  auto GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) -> bool;

  void Resize(size_t initial_size);

  auto GetSize() -> size_t;

 private:
  auto GetHeaderPage() -> HashTableHeaderPage *;
  auto GetBlockPage(page_id_t block_page_id) -> HASH_TABLE_BLOCK_TYPE *;
  void ResizeInsert(HashTableHeaderPage *header_page, const KeyType &key, const ValueType &value);
  void DeleteBlockPages(HashTableHeaderPage *old_header_page);
  void CreateNewBlockPages(HashTableHeaderPage *header_page, size_t num_blocks);
  auto GetValueLatchFree(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) -> bool;

  // member variable
  page_id_t header_page_id_;
  BufferPoolManager *buffer_pool_manager_;
  KeyComparator comparator_;

  // Readers includes inserts and removes, writer is only resize
  ReaderWriterLatch table_latch_;

  // Hash function
  HashFunction<KeyType> hash_fn_;
};

}  // namespace bustub
