//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_bucket_page.cpp
//
// Identification: src/storage/page/extendible_htable_bucket_page.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <optional>
#include <utility>

#include "common/exception.h"
#include "storage/page/extendible_htable_bucket_page.h"

namespace bustub {

/**
 * After creating a new bucket page from buffer pool, must call initialize
 * method to set default values
 * @param max_size Max size of the bucket array
 */
template <typename K, typename V, typename KC>
void ExtendibleHTableBucketPage<K, V, KC>::Init(uint32_t max_size) {
  throw NotImplementedException("ExtendibleHTableBucketPage not implemented");
}

/**
 * Lookup a key
 *
 * @param key key to lookup
 * @param[out] value value to set
 * @param cmp the comparator
 * @return true if the key and value are present, false if not found.
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::Lookup(const K &key, V &value, const KC &cmp) const -> bool {
  return false;
}

/**
 * Attempts to insert a key and value in the bucket.
 *
 * @param key key to insert
 * @param value value to insert
 * @param cmp the comparator to use
 * @return true if inserted, false if bucket is full or the same key is already present
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::Insert(const K &key, const V &value, const KC &cmp) -> bool {
  return false;
}

/**
 * Removes a key and value.
 *
 * @return true if removed, false if not found
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::Remove(const K &key, const KC &cmp) -> bool {
  return false;
}

template <typename K, typename V, typename KC>
void ExtendibleHTableBucketPage<K, V, KC>::RemoveAt(uint32_t bucket_idx) {
  throw NotImplementedException("ExtendibleHTableBucketPage not implemented");
}

/**
 * @brief Gets the key at an index in the bucket.
 *
 * @param bucket_idx the index in the bucket to get the key at
 * @return key at index bucket_idx of the bucket
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::KeyAt(uint32_t bucket_idx) const -> K {
  return {};
}

/**
 * Gets the value at an index in the bucket.
 *
 * @param bucket_idx the index in the bucket to get the value at
 * @return value at index bucket_idx of the bucket
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::ValueAt(uint32_t bucket_idx) const -> V {
  return {};
}

/**
 * Gets the entry at an index in the bucket.
 *
 * @param bucket_idx the index in the bucket to get the entry at
 * @return entry at index bucket_idx of the bucket
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::EntryAt(uint32_t bucket_idx) const -> const std::pair<K, V> & {
  return array_[0];
}

/**
 * @return number of entries in the bucket
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::Size() const -> uint32_t {
  return 0;
}

/**
 * @return whether the bucket is full
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::IsFull() const -> bool {
  return false;
}

/**
 * @return whether the bucket is empty
 */
template <typename K, typename V, typename KC>
auto ExtendibleHTableBucketPage<K, V, KC>::IsEmpty() const -> bool {
  return false;
}

template class ExtendibleHTableBucketPage<int, int, IntComparator>;
template class ExtendibleHTableBucketPage<GenericKey<4>, RID, GenericComparator<4>>;
template class ExtendibleHTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>;
template class ExtendibleHTableBucketPage<GenericKey<16>, RID, GenericComparator<16>>;
template class ExtendibleHTableBucketPage<GenericKey<32>, RID, GenericComparator<32>>;
template class ExtendibleHTableBucketPage<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
