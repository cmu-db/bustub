//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_bucket_page.h
//
// Identification: src/include/storage/page/hash_table_bucket_page.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <utility>
#include <vector>

#include "common/config.h"
#include "storage/index/int_comparator.h"
#include "storage/page/hash_table_page_defs.h"

namespace bustub {
/**
 * Store indexed key and and value together within bucket page. Supports
 * non-unique keys.
 *
 * Bucket page format (keys are stored in order):
 *  ----------------------------------------------------------------
 * | KEY(1) + VALUE(1) | KEY(2) + VALUE(2) | ... | KEY(n) + VALUE(n)
 *  ----------------------------------------------------------------
 *
 *  Here '+' means concatenation.
 *  The above format omits the space required for the occupied_ and
 *  readable_ arrays. More information is in storage/page/hash_table_page_defs.h.
 *
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
class HashTableBucketPage {
 public:
  // Delete all constructor / destructor to ensure memory safety
  HashTableBucketPage() = delete;

  auto GetValue(KeyType key, KeyComparator cmp, std::vector<ValueType> *result) -> bool;

  auto Insert(KeyType key, ValueType value, KeyComparator cmp) -> bool;

  auto Remove(KeyType key, ValueType value, KeyComparator cmp) -> bool;

  auto KeyAt(uint32_t bucket_idx) const -> KeyType;

  auto ValueAt(uint32_t bucket_idx) const -> ValueType;

  void RemoveAt(uint32_t bucket_idx);

  auto IsOccupied(uint32_t bucket_idx) const -> bool;

  void SetOccupied(uint32_t bucket_idx);

  auto IsReadable(uint32_t bucket_idx) const -> bool;

  void SetReadable(uint32_t bucket_idx);

  auto NumReadable() -> uint32_t;

  auto IsFull() -> bool;

  auto IsEmpty() -> bool;

  void PrintBucket();

 private:
  //  For more on BUCKET_ARRAY_SIZE see storage/page/hash_table_page_defs.h
  char occupied_[(BUCKET_ARRAY_SIZE - 1) / 8 + 1];
  // 0 if tombstone/brand new (never occupied), 1 otherwise.
  char readable_[(BUCKET_ARRAY_SIZE - 1) / 8 + 1];
  // Flexible array member for page data.
  MappingType array_[1];
};

}  // namespace bustub
