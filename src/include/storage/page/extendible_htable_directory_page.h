//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_directory_page.h
//
// Identification: src/include/storage/page/extendible_htable_directory_page.h
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

/**
 * Directory page format:
 *  --------------------------------------------------------------------------------------
 * | MaxDepth (4) | GlobalDepth (4) | LocalDepths (512) | BucketPageIds(2048) | Free(1528)
 *  --------------------------------------------------------------------------------------
 */

#pragma once

#include <cassert>
#include <climits>
#include <cstdlib>
#include <string>

#include "common/config.h"
#include "storage/index/generic_key.h"
#include "storage/page/hash_table_page_defs.h"

namespace bustub {

static constexpr uint64_t HTABLE_DIRECTORY_PAGE_METADATA_SIZE = sizeof(uint32_t) * 2;

/**
 * HTABLE_DIRECTORY_ARRAY_SIZE is the number of page_ids that can fit in the directory page of an extendible hash index.
 * This is 512 because the directory array must grow in powers of 2, and 1024 page_ids leaves zero room for
 * storage of the other member variables: page_id_, lsn_, global_depth_, and the array local_depths_.
 * Extending the directory implementation to span multiple pages would be a meaningful improvement to the
 * implementation.
 */
static constexpr uint64_t HTABLE_DIRECTORY_MAX_DEPTH = 9;
static constexpr uint64_t HTABLE_DIRECTORY_ARRAY_SIZE = 1 << HTABLE_DIRECTORY_MAX_DEPTH;

/**
 * Directory Page for extendible hash table.
 */
class ExtendibleHTableDirectoryPage {
 public:
  // Delete all constructor / destructor to ensure memory safety
  ExtendibleHTableDirectoryPage() = delete;
  DISALLOW_COPY_AND_MOVE(ExtendibleHTableDirectoryPage);

  /**
   * After creating a new directory page from buffer pool, must call initialize
   * method to set default values
   * @param max_depth Max depth in the directory page
   */
  void Init(uint32_t max_depth = HTABLE_DIRECTORY_MAX_DEPTH);

  /**
   * Get the bucket index that the key is hashed to
   *
   * @param hash the hash of the key
   * @return bucket index current key is hashed to
   */
  auto HashToBucketIndex(uint32_t hash) -> uint32_t;

  /**
   * Lookup a bucket page using a directory index
   *
   * @param bucket_idx the index in the directory to lookup
   * @return bucket page_id corresponding to bucket_idx
   */
  auto GetBucketPageId(uint32_t bucket_idx) -> page_id_t;

  /**
   * Updates the directory index using a bucket index and page_id
   *
   * @param bucket_idx directory index at which to insert page_id
   * @param bucket_page_id page_id to insert
   */
  void SetBucketPageId(uint32_t bucket_idx, page_id_t bucket_page_id);

  /**
   * Gets the split image of an index
   *
   * @param bucket_idx the directory index for which to find the split image
   * @return the directory index of the split image
   **/
  auto GetSplitImageIndex(uint32_t bucket_idx) -> uint32_t;

  /**
   * Get the global depth of the hash table directory
   *
   * @return the global depth of the directory
   */
  auto GetGlobalDepth() -> uint32_t;

  /**
   * Increment the global depth of the directory
   */
  void IncrGlobalDepth();

  /**
   * Decrement the global depth of the directory
   */
  void DecrGlobalDepth();

  /**
   * @return true if the directory can be shrunk
   */
  auto CanShrink() -> bool;

  /**
   * @return the current directory size
   */
  auto Size() -> uint32_t;

  /**
   * Gets the local depth of the bucket at bucket_idx
   *
   * @param bucket_idx the bucket index to lookup
   * @return the local depth of the bucket at bucket_idx
   */
  auto GetLocalDepth(uint32_t bucket_idx) -> uint32_t;

  /**
   * Set the local depth of the bucket at bucket_idx to local_depth
   *
   * @param bucket_idx bucket index to update
   * @param local_depth new local depth
   */
  void SetLocalDepth(uint32_t bucket_idx, uint8_t local_depth);

  /**
   * Increment the local depth of the bucket at bucket_idx
   * @param bucket_idx bucket index to increment
   */
  void IncrLocalDepth(uint32_t bucket_idx);

  /**
   * Decrement the local depth of the bucket at bucket_idx
   * @param bucket_idx bucket index to decrement
   */
  void DecrLocalDepth(uint32_t bucket_idx);

  /**
   * VerifyIntegrity
   *
   * Verify the following invariants:
   * (1) All LD <= GD.
   * (2) Each bucket has precisely 2^(GD - LD) pointers pointing to it.
   * (3) The LD is the same at each index with the same bucket_page_id
   */
  void VerifyIntegrity();

  /**
   * Prints the current directory
   */
  void PrintDirectory();

 private:
  uint32_t max_depth_ __attribute__((__unused__));
  uint32_t global_depth_;
  uint8_t local_depths_[HTABLE_DIRECTORY_ARRAY_SIZE];
  page_id_t bucket_page_ids_[HTABLE_DIRECTORY_ARRAY_SIZE];
};

static_assert(sizeof(page_id_t) == 4);

static_assert(sizeof(ExtendibleHTableDirectoryPage) == HTABLE_DIRECTORY_PAGE_METADATA_SIZE +
                                                           HTABLE_DIRECTORY_ARRAY_SIZE +
                                                           sizeof(page_id_t) * HTABLE_DIRECTORY_ARRAY_SIZE);

static_assert(sizeof(ExtendibleHTableDirectoryPage) <= BUSTUB_PAGE_SIZE);

}  // namespace bustub
