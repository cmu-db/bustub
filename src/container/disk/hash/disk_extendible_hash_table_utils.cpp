//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_extendible_hash_table_utils.cpp
//
// Identification: src/container/disk/hash/disk_extendible_hash_table_utils.cpp
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <iostream>

#include "container/disk/hash/disk_extendible_hash_table.h"

namespace bustub {

/**
 * Hash - simple helper to downcast MurmurHash's 64-bit hash to 32-bit
 * for extendible hashing.
 *
 * @param key the key to hash
 * @return the downcasted 32-bit hash
 */
template <typename K, typename V, typename KC>
auto DiskExtendibleHashTable<K, V, KC>::Hash(K key) const -> uint32_t {
  return static_cast<uint32_t>(hash_fn_.GetHash(key));
}

/**
 * @brief Identity Hash for testing purposes.
 */
template <>
auto DiskExtendibleHashTable<int, int, IntComparator>::Hash(int key) const -> uint32_t {
  return static_cast<uint32_t>(key);
}

template <typename K, typename V, typename KC>
void DiskExtendibleHashTable<K, V, KC>::PrintHT() const {
  std::cout << "\n";
  std::cout << "==================== PRINT! ====================\n";
  BasicPageGuard header_guard = bpm_->FetchPageBasic(header_page_id_);
  auto *header = header_guard.As<ExtendibleHTableHeaderPage>();

  header->PrintHeader();

  for (uint32_t idx = 0; idx < header->MaxSize(); idx++) {
    page_id_t directory_page_id = header->GetDirectoryPageId(idx);
    if (directory_page_id == INVALID_PAGE_ID) {
      std::cout << "Directory " << idx << ", page id: " << directory_page_id << "\n";
      continue;
    }
    BasicPageGuard directory_guard = bpm_->FetchPageBasic(directory_page_id);
    auto *directory = directory_guard.As<ExtendibleHTableDirectoryPage>();

    std::cout << "Directory " << idx << ", page id: " << directory_page_id << "\n";
    directory->PrintDirectory();

    for (uint32_t idx2 = 0; idx2 < directory->Size(); idx2++) {
      page_id_t bucket_page_id = directory->GetBucketPageId(idx2);
      BasicPageGuard bucket_guard = bpm_->FetchPageBasic(bucket_page_id);
      auto *bucket = bucket_guard.As<ExtendibleHTableBucketPage<K, V, KC>>();

      std::cout << "Bucket " << idx2 << ", page id: " << bucket_page_id << "\n";
      bucket->PrintBucket();
    }
  }
  std::cout << "==================== END OF PRINT! ====================\n";
  std::cout << "\n";
}

/*****************************************************************************
 * Verification
 *****************************************************************************/

template <typename K, typename V, typename KC>
void DiskExtendibleHashTable<K, V, KC>::VerifyIntegrity() const {
  BUSTUB_ASSERT(header_page_id_ != INVALID_PAGE_ID, "header page id is invalid");
  BasicPageGuard header_guard = bpm_->FetchPageBasic(header_page_id_);
  auto *header = header_guard.As<ExtendibleHTableHeaderPage>();

  // for each of the directory pages, check their integrity using directory page VerifyIntegrity
  for (uint32_t idx = 0; idx < header->MaxSize(); idx++) {
    auto directory_page_id = header->GetDirectoryPageId(idx);
    if (static_cast<int>(directory_page_id) != INVALID_PAGE_ID) {
      BasicPageGuard directory_guard = bpm_->FetchPageBasic(directory_page_id);
      auto *directory = directory_guard.As<ExtendibleHTableDirectoryPage>();
      directory->VerifyIntegrity();
    }
  }
}

template <typename K, typename V, typename KC>
auto DiskExtendibleHashTable<K, V, KC>::GetHeaderPageId() const -> page_id_t {
  return header_page_id_;
}

template class DiskExtendibleHashTable<int, int, IntComparator>;
template class DiskExtendibleHashTable<GenericKey<4>, RID, GenericComparator<4>>;
template class DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>>;
template class DiskExtendibleHashTable<GenericKey<16>, RID, GenericComparator<16>>;
template class DiskExtendibleHashTable<GenericKey<32>, RID, GenericComparator<32>>;
template class DiskExtendibleHashTable<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
