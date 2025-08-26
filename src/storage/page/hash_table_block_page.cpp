//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_block_page.cpp
//
// Identification: src/storage/page/hash_table_block_page.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/hash_table_block_page.h"
#include "storage/index/generic_key.h"

namespace bustub {

/**
 * Gets the key at an index in the block.
 *
 * @param bucket_ind the index in the block to get the key at
 * @return key at index bucket_ind of the block
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BLOCK_TYPE::KeyAt(slot_offset_t bucket_ind) const -> KeyType {
  return {};
}

/**
 * Gets the value at an index in the block.
 *
 * @param bucket_ind the index in the block to get the value at
 * @return value at index bucket_ind of the block
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BLOCK_TYPE::ValueAt(slot_offset_t bucket_ind) const -> ValueType {
  return {};
}

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
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BLOCK_TYPE::Insert(slot_offset_t bucket_ind, const KeyType &key, const ValueType &value) -> bool {
  return false;
}

/**
 * Removes a key and value at index.
 *
 * @param bucket_ind ind to remove the value
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_BLOCK_TYPE::Remove(slot_offset_t bucket_ind) {}

/**
 * Returns whether or not an index is occupied (key/value pair or tombstone)
 *
 * @param bucket_ind index to look at
 * @return true if the index is occupied, false otherwise
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BLOCK_TYPE::IsOccupied(slot_offset_t bucket_ind) const -> bool {
  return false;
}

/**
 * Returns whether or not an index is readable (valid key/value pair)
 *
 * @param bucket_ind index to look at
 * @return true if the index is readable, false otherwise
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
auto HASH_TABLE_BLOCK_TYPE::IsReadable(slot_offset_t bucket_ind) const -> bool {
  return false;
}

// DO NOT REMOVE ANYTHING BELOW THIS LINE
template class HashTableBlockPage<int, int, IntComparator>;
template class HashTableBlockPage<GenericKey<4>, RID, GenericComparator<4>>;
template class HashTableBlockPage<GenericKey<8>, RID, GenericComparator<8>>;
template class HashTableBlockPage<GenericKey<16>, RID, GenericComparator<16>>;
template class HashTableBlockPage<GenericKey<32>, RID, GenericComparator<32>>;
template class HashTableBlockPage<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
