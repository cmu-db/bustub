//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_directory_page.cpp
//
// Identification: src/storage/page/extendible_htable_directory_page.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/extendible_htable_directory_page.h"

#include <algorithm>
#include <unordered_map>

#include "common/config.h"
#include "common/logger.h"

namespace bustub {

/**
 * After creating a new directory page from buffer pool, must call initialize
 * method to set default values
 * @param max_depth Max depth in the directory page
 */
void ExtendibleHTableDirectoryPage::Init(uint32_t max_depth) {
  throw NotImplementedException("ExtendibleHTableDirectoryPage is not implemented");
}

/**
 * Get the bucket index that the key is hashed to
 *
 * @param hash the hash of the key
 * @return bucket index current key is hashed to
 */
auto ExtendibleHTableDirectoryPage::HashToBucketIndex(uint32_t hash) const -> uint32_t { return 0; }

/**
 * Lookup a bucket page using a directory index
 *
 * @param bucket_idx the index in the directory to lookup
 * @return bucket page_id corresponding to bucket_idx
 */
auto ExtendibleHTableDirectoryPage::GetBucketPageId(uint32_t bucket_idx) const -> page_id_t { return INVALID_PAGE_ID; }

/**
 * Updates the directory index using a bucket index and page_id
 *
 * @param bucket_idx directory index at which to insert page_id
 * @param bucket_page_id page_id to insert
 */
void ExtendibleHTableDirectoryPage::SetBucketPageId(uint32_t bucket_idx, page_id_t bucket_page_id) {
  throw NotImplementedException("ExtendibleHTableDirectoryPage is not implemented");
}

/**
 * Gets the split image of an index
 *
 * @param bucket_idx the directory index for which to find the split image
 * @return the directory index of the split image
 **/
auto ExtendibleHTableDirectoryPage::GetSplitImageIndex(uint32_t bucket_idx) const -> uint32_t { return 0; }

/**
 * Get the global depth of the hash table directory
 *
 * @return the global depth of the directory
 */
auto ExtendibleHTableDirectoryPage::GetGlobalDepth() const -> uint32_t { return 0; }

/**
 * Increment the global depth of the directory
 */
void ExtendibleHTableDirectoryPage::IncrGlobalDepth() {
  throw NotImplementedException("ExtendibleHTableDirectoryPage is not implemented");
}

/**
 * Decrement the global depth of the directory
 */
void ExtendibleHTableDirectoryPage::DecrGlobalDepth() {
  throw NotImplementedException("ExtendibleHTableDirectoryPage is not implemented");
}

/**
 * @return true if the directory can be shrunk
 */
auto ExtendibleHTableDirectoryPage::CanShrink() -> bool { return false; }

/**
 * @return the current directory size
 */
auto ExtendibleHTableDirectoryPage::Size() const -> uint32_t { return 0; }

/**
 * Gets the local depth of the bucket at bucket_idx
 *
 * @param bucket_idx the bucket index to lookup
 * @return the local depth of the bucket at bucket_idx
 */
auto ExtendibleHTableDirectoryPage::GetLocalDepth(uint32_t bucket_idx) const -> uint32_t { return 0; }

/**
 * Set the local depth of the bucket at bucket_idx to local_depth
 *
 * @param bucket_idx bucket index to update
 * @param local_depth new local depth
 */
void ExtendibleHTableDirectoryPage::SetLocalDepth(uint32_t bucket_idx, uint8_t local_depth) {
  throw NotImplementedException("ExtendibleHTableDirectoryPage is not implemented");
}

/**
 * Increment the local depth of the bucket at bucket_idx
 * @param bucket_idx bucket index to increment
 */
void ExtendibleHTableDirectoryPage::IncrLocalDepth(uint32_t bucket_idx) {
  throw NotImplementedException("ExtendibleHTableDirectoryPage is not implemented");
}

/**
 * Decrement the local depth of the bucket at bucket_idx
 * @param bucket_idx bucket index to decrement
 */
void ExtendibleHTableDirectoryPage::DecrLocalDepth(uint32_t bucket_idx) {
  throw NotImplementedException("ExtendibleHTableDirectoryPage is not implemented");
}

}  // namespace bustub
