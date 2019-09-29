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
INDEX_TEMPLATE_ARGUMENTS
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
  KeyType KeyAt(size_t bucket_ind) const;

  /**
   * Gets the value at an index in the block.
   *
   * @param bucket_ind the index in the block to get the value at
   * @return value at index bucket_ind of the block
   */
  ValueType ValueAt(size_t bucket_ind) const;

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
  bool Insert(size_t bucket_ind, const KeyType &key, const ValueType &value);

  /**
   * Removes a key and value at index.
   *
   * @param bucket_ind ind to remove the value
   */
  void Remove(size_t bucket_ind);

  /**
   * Returns whether or not an index is occupied (key/value pair or tombstone)
   *
   * @param bucket_ind index to look at
   * @return true if the index is occupied, false otherwise
   */
  bool IsOccupied(size_t bucket_ind);

  /**
   * Returns whether or not an index is readable (valid key/value pair)
   *
   * @param bucket_ind index to look at
   * @return true if the index is readable, false otherwise
   */
  bool IsReadable(size_t bucket_ind);

 private:
  std::atomic_char occupied_[(BLOCK_ARRAY_SIZE - 1) / 8 + 1];

  // 0 if tombstone/brand new (never occupied), 1 otherwise.
  std::atomic_char readable_[(BLOCK_ARRAY_SIZE - 1) / 8 + 1];
  MappingType array_[0];
};

}  // namespace bustub
