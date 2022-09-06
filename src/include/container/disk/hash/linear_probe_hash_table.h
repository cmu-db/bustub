//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// linear_probe_hash_table.h
//
// Identification: src/include/container/disk/hash/linear_probe_hash_table.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
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
  /**
   * Creates a new LinearProbeHashTable
   *
   * @param buffer_pool_manager buffer pool manager to be used
   * @param comparator comparator for keys
   * @param num_buckets initial number of buckets contained by this hash table
   * @param hash_fn the hash function
   */
  explicit LinearProbeHashTable(const std::string &name, BufferPoolManager *buffer_pool_manager,
                                const KeyComparator &comparator, size_t num_buckets, HashFunction<KeyType> hash_fn);

  /**
   * Inserts a key-value pair into the hash table.
   * @param transaction the current transaction
   * @param key the key to create
   * @param value the value to be associated with the key
   * @return true if insert succeeded, false otherwise
   */
  auto Insert(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool;

  /**
   * Deletes the associated value for the given key.
   * @param transaction the current transaction
   * @param key the key to delete
   * @param value the value to delete
   * @return true if remove succeeded, false otherwise
   */
  auto Remove(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool;

  /**
   * Performs a point query on the hash table.
   * @param transaction the current transaction
   * @param key the key to look up
   * @param[out] result the value(s) associated with a given key
   * @return the value(s) associated with the given key
   */
  auto GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) -> bool;

  /**
   * Resizes the table to at least twice the initial size provided.
   * @param initial_size the initial size of the hash table
   */
  void Resize(size_t initial_size);

  /**
   * Gets the size of the hash table
   * @return current size of the hash table
   */
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
