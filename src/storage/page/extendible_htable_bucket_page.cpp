//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_bucket_page.cpp
//
// Identification: src/storage/page/extendible_htable_bucket_page.cpp
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/extendible_htable_bucket_page.h"
#include <optional>

namespace bustub {

template <typename KeyType, typename ValueType, typename KeyComparator>
void ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::Init(uint32_t max_size) {
  throw NotImplementedException("ExtendibleHTableBucketPage is not implemented");
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::Lookup(const KeyType &key, ValueType &value,
                                                                           const KeyComparator &cmp) const -> bool {
  return false;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::Insert(const KeyType &key, const ValueType &value,
                                                                           const KeyComparator &cmp) -> bool {
  return false;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::Remove(const KeyType &key, const KeyComparator &cmp)
    -> bool {
  return false;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::KeyAt(uint32_t bucket_idx) const -> KeyType {
  return {};
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::ValueAt(uint32_t bucket_idx) const -> ValueType {
  return {};
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::IsFull() -> bool {
  return false;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
auto ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::IsEmpty() -> bool {
  return false;
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void ExtendibleHTableBucketPage<KeyType, ValueType, KeyComparator>::PrintBucket() {
  throw NotImplementedException("PrintBucket is not implemented");
}

template class ExtendibleHTableBucketPage<GenericKey<4>, RID, GenericComparator<4>>;

template class ExtendibleHTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>;

template class ExtendibleHTableBucketPage<GenericKey<16>, RID, GenericComparator<16>>;

template class ExtendibleHTableBucketPage<GenericKey<32>, RID, GenericComparator<32>>;

template class ExtendibleHTableBucketPage<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
