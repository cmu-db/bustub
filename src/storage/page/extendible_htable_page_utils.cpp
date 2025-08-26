//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_page_utils.cpp
//
// Identification: src/storage/page/extendible_htable_page_utils.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "common/logger.h"
#include "common/macros.h"
#include "storage/page/extendible_htable_bucket_page.h"
#include "storage/page/extendible_htable_directory_page.h"
#include "storage/page/extendible_htable_header_page.h"

namespace bustub {

/**
 * Prints the header's occupancy information
 */
void ExtendibleHTableHeaderPage::PrintHeader() const {
  LOG_DEBUG("======== HEADER (max_depth_: %u) ========", max_depth_);
  LOG_DEBUG("| directory_idx | page_id |");
  for (uint32_t idx = 0; idx < static_cast<uint32_t>(1 << max_depth_); idx++) {
    LOG_DEBUG("|    %u    |    %u    |", idx, directory_page_ids_[idx]);
  }
  LOG_DEBUG("======== END HEADER ========");
}

/**
 * Prints the current directory
 */
void ExtendibleHTableDirectoryPage::PrintDirectory() const {
  LOG_DEBUG("======== DIRECTORY (global_depth_: %u) ========", global_depth_);
  LOG_DEBUG("| bucket_idx | page_id | local_depth |");
  for (uint32_t idx = 0; idx < static_cast<uint32_t>(0x1 << global_depth_); idx++) {
    LOG_DEBUG("|    %u    |    %u    |    %u    |", idx, bucket_page_ids_[idx], local_depths_[idx]);
  }
  LOG_DEBUG("================ END DIRECTORY ================");
}

/**
 * Prints the bucket's occupancy information
 */
template <typename K, typename V, typename KC>
void ExtendibleHTableBucketPage<K, V, KC>::PrintBucket() const {
  std::cout << "======== BUCKET (size_: " << size_ << " | max_size_: " << max_size_ << ") ========\n";
  std::cout << ("| i | k | v |\n");
  for (uint32_t idx = 0; idx < size_; idx++) {
    std::cout << "| " << idx << " | " << KeyAt(idx) << " | " << ValueAt(idx) << " |\n";
  }
  std::cout << "================ END BUCKET ================\n";
  std::cout << "\n";
}

/**
 * VerifyIntegrity
 *
 * Verify the following invariants:
 * (1) All LD <= GD.
 * (2) Each bucket has precisely 2^(GD - LD) pointers pointing to it.
 * (3) The LD is the same at each index with the same bucket_page_id
 */
void ExtendibleHTableDirectoryPage::VerifyIntegrity() const {
  // build maps of {bucket_page_id : pointer_count} and {bucket_page_id : local_depth}
  std::unordered_map<page_id_t, uint32_t> page_id_to_count = std::unordered_map<page_id_t, uint32_t>();
  std::unordered_map<page_id_t, uint32_t> page_id_to_ld = std::unordered_map<page_id_t, uint32_t>();

  // Verify: (3) The LD is the same at each index with the same bucket_page_id
  for (uint32_t curr_idx = 0; curr_idx < Size(); curr_idx++) {
    page_id_t curr_page_id = bucket_page_ids_[curr_idx];
    uint32_t curr_ld = local_depths_[curr_idx];

    // Verify: (1) All LD <= GD.
    BUSTUB_ASSERT(curr_ld <= global_depth_, "there exists a local depth greater than the global depth");

    ++page_id_to_count[curr_page_id];

    if (page_id_to_ld.count(curr_page_id) > 0 && curr_ld != page_id_to_ld[curr_page_id]) {
      uint32_t old_ld = page_id_to_ld[curr_page_id];
      LOG_WARN("Verify Integrity: curr_local_depth: %u, old_local_depth %u, for page_id: %u", curr_ld, old_ld,
               curr_page_id);
      PrintDirectory();
      BUSTUB_ASSERT(curr_ld == page_id_to_ld[curr_page_id],
                    "local depth is not the same at each index with same bucket page id");
    } else {
      page_id_to_ld[curr_page_id] = curr_ld;
    }
  }

  // Verify: (2) Each bucket has precisely 2^(GD - LD) pointers pointing to it.
  auto it = page_id_to_count.begin();
  while (it != page_id_to_count.end()) {
    page_id_t curr_page_id = it->first;
    uint32_t curr_count = it->second;
    uint32_t curr_ld = page_id_to_ld[curr_page_id];
    uint32_t required_count = 0x1 << (global_depth_ - curr_ld);

    if (curr_count != required_count) {
      LOG_WARN("Verify Integrity: curr_count: %u, required_count %u, for page_id: %u", curr_count, required_count,
               curr_page_id);
      PrintDirectory();
      BUSTUB_ASSERT(curr_count == required_count, "a bucket does not have precisely 2^(GD - LD) pointers to it");
    }
    it++;
  }
}

template class ExtendibleHTableBucketPage<int, int, IntComparator>;
template class ExtendibleHTableBucketPage<GenericKey<4>, RID, GenericComparator<4>>;
template class ExtendibleHTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>;
template class ExtendibleHTableBucketPage<GenericKey<16>, RID, GenericComparator<16>>;
template class ExtendibleHTableBucketPage<GenericKey<32>, RID, GenericComparator<32>>;
template class ExtendibleHTableBucketPage<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
