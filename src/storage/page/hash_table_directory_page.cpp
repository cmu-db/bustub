//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_header_page.cpp
//
// Identification: src/storage/page/hash_table_header_page.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/hash_table_directory_page.h"
#include <algorithm>
#include "common/logger.h"

namespace bustub {
page_id_t HashTableDirectoryPage::GetPageId() const { return page_id_; }

void HashTableDirectoryPage::SetPageId(bustub::page_id_t page_id) { page_id_ = page_id; }

lsn_t HashTableDirectoryPage::GetLSN() const { return lsn_; }

void HashTableDirectoryPage::SetLSN(lsn_t lsn) { lsn_ = lsn; }

uint32_t HashTableDirectoryPage::GetGlobalDepth() { return global_depth_; }

void HashTableDirectoryPage::IncrGlobalDepth() {}

void HashTableDirectoryPage::DecrGlobalDepth() { global_depth_--; }

page_id_t HashTableDirectoryPage::GetBucketPageId(uint32_t bucket_idx) { return 0; }

void HashTableDirectoryPage::SetBucketPageId(uint32_t bucket_idx, page_id_t bucket_page_id) {}

uint32_t HashTableDirectoryPage::Size() { return 0; }

bool HashTableDirectoryPage::CanShrink() { return false; }

uint32_t HashTableDirectoryPage::GetLocalDepth(uint32_t bucket_idx) { return 0; }

void HashTableDirectoryPage::SetLocalDepth(uint32_t bucket_idx, uint8_t local_depth) {}

void HashTableDirectoryPage::IncrLocalDepth(uint32_t bucket_idx) {}

void HashTableDirectoryPage::DecrLocalDepth(uint32_t bucket_idx) {}

uint32_t HashTableDirectoryPage::GetLocalHighBit(uint32_t bucket_idx) { return 0; }

void HashTableDirectoryPage::PrintDirectory() {
  LOG_DEBUG("======== DIRECTORY (global_depth_: %u) ========", global_depth_);
  LOG_DEBUG("| bucket_idx | page_id | local_depth |");
  for (uint32_t idx = 0; idx < static_cast<uint32_t>(0x1 << global_depth_); idx++) {
    LOG_DEBUG("|      %u     |     %u     |     %u     |", idx, bucket_page_ids_[idx], local_depths_[idx]);
  }
  LOG_DEBUG("================ END DIRECTORY ================");
}

}  // namespace bustub
