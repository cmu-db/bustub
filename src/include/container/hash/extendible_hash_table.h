//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_hash_table.h
//
// Identification: src/include/container/hash/extendible_hash_table.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <queue>
#include <string>
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "concurrency/transaction.h"
#include "container/hash/hash_function.h"
#include "storage/page/hash_table_bucket_page.h"
#include "storage/page/hash_table_directory_page.h"

namespace bustub {

#define HASH_TABLE_TYPE ExtendibleHashTable<KeyType, ValueType, KeyComparator>

/**
 * Implementation of extendible hash table that is backed by a buffer pool
 * manager. Non-unique keys are supported. Supports insert and delete. The
 * table grows/shrinks dynamically as buckets become full/empty.
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
class ExtendibleHashTable {
 public:
  /**
   * Creates a new ExtendibleHashTable.
   *
   * @param buffer_pool_manager buffer pool manager to be used
   * @param comparator comparator for keys
   * @param hash_fn the hash function
   */
  explicit ExtendibleHashTable(const std::string &name, BufferPoolManager *buffer_pool_manager,
                               const KeyComparator &comparator, HashFunction<KeyType> hash_fn);

  /**
   * Inserts a key-value pair into the hash table.
   *
   * @param transaction the current transaction
   * @param key the key to create
   * @param value the value to be associated with the key
   * @return true if insert succeeded, false otherwise
   */
  bool Insert(Transaction *transaction, const KeyType &key, const ValueType &value);

  /**
   * Deletes the associated value for the given key.
   *
   * @param transaction the current transaction
   * @param key the key to delete
   * @param value the value to delete
   * @return true if remove succeeded, false otherwise
   */
  bool Remove(Transaction *transaction, const KeyType &key, const ValueType &value);

  /**
   * Performs a point query on the hash table.
   *
   * @param transaction the current transaction
   * @param key the key to look up
   * @param[out] result the value(s) associated with a given key
   * @return the value(s) associated with the given key
   */
  bool GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result);

 private:
  /**
   * Hashes a key and downcasts to uint32_t.
   *
   * @param key the key to hash
   * @return the 32-bit hash
   */
  inline uint32_t Hash(KeyType key);

  // member variables
  page_id_t directory_page_id_;
  BufferPoolManager *buffer_pool_manager_;
  KeyComparator comparator_;

  // Readers includes inserts and removes, writers are splits and merges
  ReaderWriterLatch table_latch_;
  HashFunction<KeyType> hash_fn_;
};

}  // namespace bustub
