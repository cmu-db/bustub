//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_hash_table.h
//
// Identification: src/include/container/disk/hash/disk_hash_table.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <queue>
#include <string>
#include <vector>

#include "concurrency/transaction.h"
#include "storage/page/hash_table_page_defs.h"

namespace bustub {

template <typename KeyType, typename ValueType, typename KeyComparator>
class DiskHashTable {
 public:
  virtual ~DiskHashTable() = default;

  /**
   * Inserts a key-value pair into the hash table.
   * @param transaction the current transaction
   * @param key the key to create
   * @param value the value to be associated with the key
   * @return true if insert succeeded, false otherwise
   */
  virtual auto Insert(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool = 0;

  /**
   * Deletes the associated value for the given key.
   * @param transaction the current transaction
   * @param key the key to delete
   * @param value the value to delete
   * @return true if remove succeeded, false otherwise
   */
  virtual auto Remove(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool = 0;

  /**
   * Performs a point query on the hash table.
   * @param transaction the current transaction
   * @param key the key to look up
   * @param[out] result the value(s) associated with a given key
   * @return the value(s) associated with the given key
   */
  virtual auto GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) -> bool = 0;
};

}  // namespace bustub
