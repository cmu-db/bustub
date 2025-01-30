//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_block_page.h
//
// Identification: src/include/storage/page/hash_table_block_page.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <utility>
#include <vector>

#include "common/config.h"
#include "storage/index/int_comparator.h"
#include "storage/page/hash_table_page_defs.h"

namespace bustub {
/**
 * Store indexed key and and value together within block page. Supports
 * non-unique keys.
 *
 * Block page format (keys are stored in order):
 *  ----------------------------------------------------------------
 * | KEY(1) + VALUE(1) | KEY(2) + VALUE(2) | ... | KEY(n) + VALUE(n)
 *  ----------------------------------------------------------------
 *
 *  Here '+' means concatenation.
 *
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
class HashTableBlockPage {
 public:
  // Delete all constructor / destructor to ensure memory safety
  HashTableBlockPage() = delete;

  auto KeyAt(slot_offset_t bucket_ind) const -> KeyType;

  auto ValueAt(slot_offset_t bucket_ind) const -> ValueType;

  auto Insert(slot_offset_t bucket_ind, const KeyType &key, const ValueType &value) -> bool;

  void Remove(slot_offset_t bucket_ind);

  auto IsOccupied(slot_offset_t bucket_ind) const -> bool;

  auto IsReadable(slot_offset_t bucket_ind) const -> bool;

  /**
   * Scan the bucket and collect values that have the matching key
   *
   * @return true if at least one key matched
   */
  auto GetValue(KeyType key, KeyComparator cmp, std::vector<ValueType> *result) -> bool;

  /**
   * Attempts to insert a key and value in the bucket.
   * The insert is thread safe. It uses compare and swap to claim the index,
   * and then writes the key and value into the index, and then marks the
   * index as readable.
   *
   * @param key key to insert
   * @param value value to insert
   * @return true if inserted, false if duplicate KV pair or bucket is full
   */
  auto Insert(KeyType key, ValueType value, KeyComparator cmp) -> bool;

  /**
   * Removes a key and value.
   * @return true if removed, false if not found
   */
  auto Remove(KeyType key, ValueType value, KeyComparator cmp) -> bool;

  /**
   * @return the number of readable elements, i.e. current size
   */
  auto NumReadable() -> uint32_t;

  /**
   * @return whether the bucket is full
   */
  auto IsFull() -> bool;

  /**
   * @return whether the bucket is empty
   */
  auto IsEmpty() -> bool;

  void PrintBucket();

 private:
  std::atomic_char occupied_[(BLOCK_ARRAY_SIZE - 1) / 8 + 1];

  // 0 if tombstone/brand new (never occupied), 1 otherwise.
  std::atomic_char readable_[(BLOCK_ARRAY_SIZE - 1) / 8 + 1];
  // Flexible array member for page data.
  MappingType array_[1];
};

}  // namespace bustub
