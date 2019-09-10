//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table.h
//
// Identification: src/include/container/hash/hash_table.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

namespace bustub {

template <typename K, typename V>
class HashTable {
 public:
  virtual ~HashTable() = default;

  /**
   * Find the value associated with a key.
   * @param key key to lookup
   * @param[out] value output value
   * @return true iff the key was found
   */
  virtual bool Find(const K &key, V *value) = 0;

  /**
   * Remove a key from the hash table
   * @param key key to remove
   * @return true iff the key was found and removed
   */
  virtual bool Remove(const K &key) = 0;

  /**
   * Insert a key-value pair in the hash table. You can assume that the key is not duplicated.
   * @param key key to insert
   * @param value value associated with the key
   */
  virtual void Insert(const K &key, const V &value) = 0;
};

}  // namespace bustub
