//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_directory_page.h
//
// Identification: src/include/storage/page/hash_table_directory_page.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <climits>
#include <cstdlib>
#include <string>

#include "storage/index/generic_key.h"
#include "storage/page/hash_table_page_defs.h"

namespace bustub {

/**
 *
 * Directory Page for extendible hash table.
 *
 * Directory format (size in byte):
 * --------------------------------------------------------------------------------------------
 * | LSN (4) | PageId(4) | GlobalDepth(4) | LocalDepths(512) | BucketPageIds(2048) | Free(1524)
 * --------------------------------------------------------------------------------------------
 */
class HashTableDirectoryPage {
 public:
  /**
   * @return the page ID of this page
   */
  auto GetPageId() const -> page_id_t;

  /**
   * Sets the page ID of this page
   *
   * @param page_id the page id to which to set the page_id_ field
   */
  void SetPageId(page_id_t page_id);

  /**
   * @return the lsn of this page
   */
  auto GetLSN() const -> lsn_t;

  /**
   * Sets the LSN of this page
   *
   * @param lsn the log sequence number to which to set the lsn field
   */
  void SetLSN(lsn_t lsn);

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
   * GetGlobalDepthMask - returns a mask of global_depth 1's and the rest 0's.
   *
   * In Extendible Hashing we map a key to a directory index
   * using the following hash + mask function.
   *
   * DirectoryIndex = Hash(key) & GLOBAL_DEPTH_MASK
   *
   * where GLOBAL_DEPTH_MASK is a mask with exactly GLOBAL_DEPTH 1's from LSB
   * upwards.  For example, global depth 3 corresponds to 0x00000007 in a 32-bit
   * representation.
   *
   * @return mask of global_depth 1's and the rest 0's (with 1's from LSB upwards)
   */
  auto GetGlobalDepthMask() -> uint32_t;

  /**
   * GetLocalDepthMask - same as global depth mask, except it
   * uses the local depth of the bucket located at bucket_idx
   *
   * @param bucket_idx the index to use for looking up local depth
   * @return mask of local 1's and the rest 0's (with 1's from LSB upwards)
   */
  auto GetLocalDepthMask(uint32_t bucket_idx) -> uint32_t;

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
   * Gets the high bit corresponding to the bucket's local depth.
   * This is not the same as the bucket index itself.  This method
   * is helpful for finding the pair, or "split image", of a bucket.
   *
   * @param bucket_idx bucket index to lookup
   * @return the high bit corresponding to the bucket's local depth
   */
  auto GetLocalHighBit(uint32_t bucket_idx) -> uint32_t;

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
  page_id_t page_id_;
  lsn_t lsn_;
  uint32_t global_depth_{0};
  uint8_t local_depths_[DIRECTORY_ARRAY_SIZE];
  page_id_t bucket_page_ids_[DIRECTORY_ARRAY_SIZE];
};

}  // namespace bustub
