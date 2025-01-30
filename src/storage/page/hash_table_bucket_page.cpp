//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_bucket_page.cpp
//
// Identification: src/storage/page/hash_table_bucket_page.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/hash_table_bucket_page.h"
#include "common/logger.h"
#include "common/util/hash_util.h"
#include "storage/index/generic_key.h"
#include "storage/index/hash_comparator.h"
#include "storage/table/tmp_tuple.h"

namespace bustub {

/**
 * Scan the bucket and collect values that have the matching key
 *
 * @return true if at least one key matched
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::GetValue(KeyType key, KeyComparator cmp, std::vector<ValueType> *result) -> bool {
  return false;
}

/**
 * Attempts to insert a key and value in the bucket.  Uses the occupied_
 * and readable_ arrays to keep track of each slot's availability.
 *
 * @param key key to insert
 * @param value value to insert
 * @return true if inserted, false if duplicate KV pair or bucket is full
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::Insert(KeyType key, ValueType value, KeyComparator cmp) -> bool {
  return true;
}

/**
 * Removes a key and value.
 *
 * @return true if removed, false if not found
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::Remove(KeyType key, ValueType value, KeyComparator cmp) -> bool {
  return false;
}

/**
 * Gets the key at an index in the bucket.
 *
 * @param bucket_idx the index in the bucket to get the key at
 * @return key at index bucket_idx of the bucket
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::KeyAt(uint32_t bucket_idx) const -> KeyType {
  return {};
}

/**
 * Gets the value at an index in the bucket.
 *
 * @param bucket_idx the index in the bucket to get the value at
 * @return value at index bucket_idx of the bucket
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::ValueAt(uint32_t bucket_idx) const -> ValueType {
  return {};
}

/**
 * Remove the KV pair at bucket_idx
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::RemoveAt(uint32_t bucket_idx) {}

/**
 * Returns whether or not an index is occupied (key/value pair or tombstone)
 *
 * @param bucket_idx index to look at
 * @return true if the index is occupied, false otherwise
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsOccupied(uint32_t bucket_idx) const -> bool {
  return false;
}

/**
 * SetOccupied - Updates the bitmap to indicate that the entry at
 * bucket_idx is occupied.
 *
 * @param bucket_idx the index to update
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::SetOccupied(uint32_t bucket_idx) {}

/**
 * Returns whether or not an index is readable (valid key/value pair)
 *
 * @param bucket_idx index to lookup
 * @return true if the index is readable, false otherwise
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsReadable(uint32_t bucket_idx) const -> bool {
  return false;
}

/**
 * SetReadable - Updates the bitmap to indicate that the entry at
 * bucket_idx is readable.
 *
 * @param bucket_idx the index to update
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::SetReadable(uint32_t bucket_idx) {}

/**
 * @return whether the bucket is full
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsFull() -> bool {
  return false;
}

/**
 * @return the number of readable elements, i.e. current size
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::NumReadable() -> uint32_t {
  return 0;
}

/**
 * @return whether the bucket is empty
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BUCKET_TYPE::IsEmpty() -> bool {
  return false;
}

/**
 * Prints the bucket's occupancy information
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BUCKET_TYPE::PrintBucket() {
  uint32_t size = 0;
  uint32_t taken = 0;
  uint32_t free = 0;
  for (size_t bucket_idx = 0; bucket_idx < BUCKET_ARRAY_SIZE; bucket_idx++) {
    if (!IsOccupied(bucket_idx)) {
      break;
    }

    size++;

    if (IsReadable(bucket_idx)) {
      taken++;
    } else {
      free++;
    }
  }

  LOG_INFO("Bucket Capacity: %lu, Size: %u, Taken: %u, Free: %u", BUCKET_ARRAY_SIZE, size, taken, free);
}

// DO NOT REMOVE ANYTHING BELOW THIS LINE
template class HashTableBucketPage<int, int, IntComparator>;

template class HashTableBucketPage<GenericKey<4>, RID, GenericComparator<4>>;
template class HashTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>;
template class HashTableBucketPage<GenericKey<16>, RID, GenericComparator<16>>;
template class HashTableBucketPage<GenericKey<32>, RID, GenericComparator<32>>;
template class HashTableBucketPage<GenericKey<64>, RID, GenericComparator<64>>;

// template class HashTableBucketPage<hash_t, TmpTuple, HashComparator>;

}  // namespace bustub
