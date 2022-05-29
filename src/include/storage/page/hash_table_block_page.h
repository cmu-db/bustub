//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_block_page.h
//
// Identification: src/include/storage/page/hash_table_block_page.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
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

  /**
   * Gets the key at an index in the block.
   *
   * @param bucket_ind the index in the block to get the key at
   * @return key at index bucket_ind of the block
   */
  auto KeyAt(slot_offset_t bucket_ind) const -> KeyType;

  /**
   * Gets the value at an index in the block.
   *
   * @param bucket_ind the index in the block to get the value at
   * @return value at index bucket_ind of the block
   */
  auto ValueAt(slot_offset_t bucket_ind) const -> ValueType;

  /**
   * Attempts to insert a key and value into an index in the block.
   * The insert is thread safe. It uses compare and swap to claim the index,
   * and then writes the key and value into the index, and then marks the
   * index as readable.
   *
   * @param bucket_ind index to write the key and value to
   * @param key key to insert
   * @param value value to insert
   * @return If the value is inserted successfully, it returns true. If the
   * index is marked as occupied before the key and value can be inserted,
   * Insert returns false.
   */
  auto Insert(slot_offset_t bucket_ind, const KeyType &key, const ValueType &value) -> bool;

  /**
   * Removes a key and value at index.
   *
   * @param bucket_ind ind to remove the value
   */
  void Remove(slot_offset_t bucket_ind);

  /**
   * Returns whether or not an index is occupied (key/value pair or tombstone)
   *
   * @param bucket_ind index to look at
   * @return true if the index is occupied, false otherwise
   */
  auto IsOccupied(slot_offset_t bucket_ind) const -> bool;

  /**
   * Returns whether or not an index is readable (valid key/value pair)
   *
   * @param bucket_ind index to look at
   * @return true if the index is readable, false otherwise
   */
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

  /**
   * Prints the bucket's occupancy information
   */
  void PrintBucket();

 private:
  std::atomic_char occupied_[(BLOCK_ARRAY_SIZE - 1) / 8 + 1];

  // 0 if tombstone/brand new (never occupied), 1 otherwise.
  std::atomic_char readable_[(BLOCK_ARRAY_SIZE - 1) / 8 + 1];
  // Flexible array member for page data.
  MappingType array_[1];
};

}  // namespace bustub
