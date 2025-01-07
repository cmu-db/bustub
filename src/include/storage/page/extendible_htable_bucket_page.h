//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_bucket_page.h
//
// Identification: src/include/storage/page/extendible_htable_bucket_page.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

/**
 * Bucket page format:
 *  ----------------------------------------------------------------------------
 * | METADATA | KEY(1) + VALUE(1) | KEY(2) + VALUE(2) | ... | KEY(n) + VALUE(n)
 *  ----------------------------------------------------------------------------
 *
 * Metadata format (size in byte, 8 bytes in total):
 *  --------------------------------
 * | CurrentSize (4) | MaxSize (4)
 *  --------------------------------
 */
#pragma once

#include <optional>
#include <utility>
#include <vector>

#include "common/config.h"
#include "common/macros.h"
#include "storage/index/int_comparator.h"
#include "storage/page/b_plus_tree_page.h"
#include "type/value.h"

namespace bustub {

static constexpr uint64_t HTABLE_BUCKET_PAGE_METADATA_SIZE = sizeof(uint32_t) * 2;

constexpr auto HTableBucketArraySize(uint64_t mapping_type_size) -> uint64_t {
  return (BUSTUB_PAGE_SIZE - HTABLE_BUCKET_PAGE_METADATA_SIZE) / mapping_type_size;
};

/**
 * Bucket page for extendible hash table.
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
class ExtendibleHTableBucketPage {
 public:
  // Delete all constructor / destructor to ensure memory safety
  ExtendibleHTableBucketPage() = delete;
  DISALLOW_COPY_AND_MOVE(ExtendibleHTableBucketPage);

  void Init(uint32_t max_size = HTableBucketArraySize(sizeof(MappingType)));

  auto Lookup(const KeyType &key, ValueType &value, const KeyComparator &cmp) const -> bool;

  auto Insert(const KeyType &key, const ValueType &value, const KeyComparator &cmp) -> bool;

  auto Remove(const KeyType &key, const KeyComparator &cmp) -> bool;

  void RemoveAt(uint32_t bucket_idx);

  auto KeyAt(uint32_t bucket_idx) const -> KeyType;

  auto ValueAt(uint32_t bucket_idx) const -> ValueType;

  auto EntryAt(uint32_t bucket_idx) const -> const std::pair<KeyType, ValueType> &;

  auto Size() const -> uint32_t;

  auto IsFull() const -> bool;

  auto IsEmpty() const -> bool;

  /**
   * Prints the bucket's occupancy information
   */
  void PrintBucket() const;

 private:
  uint32_t size_;
  uint32_t max_size_;
  MappingType array_[HTableBucketArraySize(sizeof(MappingType))];
};

}  // namespace bustub
