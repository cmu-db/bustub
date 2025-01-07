//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// linear_probe_hash_table.cpp
//
// Identification: src/container/disk/hash/linear_probe_hash_table.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "common/exception.h"
#include "common/logger.h"
#include "common/rid.h"
#include "container/disk/hash/linear_probe_hash_table.h"

namespace bustub {

/**
 * Creates a new LinearProbeHashTable
 *
 * @param buffer_pool_manager buffer pool manager to be used
 * @param comparator comparator for keys
 * @param num_buckets initial number of buckets contained by this hash table
 * @param hash_fn the hash function
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
HASH_TABLE_TYPE::LinearProbeHashTable(const std::string &name, BufferPoolManager *buffer_pool_manager,
                                      const KeyComparator &comparator, size_t num_buckets,
                                      HashFunction<KeyType> hash_fn)
    : buffer_pool_manager_(buffer_pool_manager), comparator_(comparator), hash_fn_(std::move(hash_fn)) {}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/

/**
 * Performs a point query on the hash table.
 * @param transaction the current transaction
 * @param key the key to look up
 * @param[out] result the value(s) associated with a given key
 * @return the value(s) associated with the given key
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) -> bool {
  return false;
}
/*****************************************************************************
 * INSERTION
 *****************************************************************************/

/**
 * Inserts a key-value pair into the hash table.
 * @param transaction the current transaction
 * @param key the key to create
 * @param value the value to be associated with the key
 * @return true if insert succeeded, false otherwise
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::Insert(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool {
  return false;
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/

/**
 * Deletes the associated value for the given key.
 * @param transaction the current transaction
 * @param key the key to delete
 * @param value the value to delete
 * @return true if remove succeeded, false otherwise
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::Remove(Transaction *transaction, const KeyType &key, const ValueType &value) -> bool {
  return false;
}

/*****************************************************************************
 * RESIZE
 *****************************************************************************/

/**
 * Resizes the table to at least twice the initial size provided.
 * @param initial_size the initial size of the hash table
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_TYPE::Resize(size_t initial_size) {}

/*****************************************************************************
 * GETSIZE
 *****************************************************************************/

/**
 * Gets the size of the hash table
 * @return current size of the hash table
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_TYPE::GetSize() -> size_t {
  return 0;
}

template class LinearProbeHashTable<int, int, IntComparator>;

template class LinearProbeHashTable<GenericKey<4>, RID, GenericComparator<4>>;
template class LinearProbeHashTable<GenericKey<8>, RID, GenericComparator<8>>;
template class LinearProbeHashTable<GenericKey<16>, RID, GenericComparator<16>>;
template class LinearProbeHashTable<GenericKey<32>, RID, GenericComparator<32>>;
template class LinearProbeHashTable<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
