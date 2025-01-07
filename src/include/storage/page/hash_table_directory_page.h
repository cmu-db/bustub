//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_directory_page.h
//
// Identification: src/include/storage/page/hash_table_directory_page.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
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
  auto GetPageId() const -> page_id_t;

  void SetPageId(page_id_t page_id);

  auto GetLSN() const -> lsn_t;

  void SetLSN(lsn_t lsn);

  auto GetBucketPageId(uint32_t bucket_idx) -> page_id_t;

  void SetBucketPageId(uint32_t bucket_idx, page_id_t bucket_page_id);

  /**
   * Gets the split image of an index
   *
   * @param bucket_idx the directory index for which to find the split image
   * @return the directory index of the split image
   **/
  auto GetSplitImageIndex(uint32_t bucket_idx) -> uint32_t;

  auto GetGlobalDepthMask() -> uint32_t;

  /**
   * GetLocalDepthMask - same as global depth mask, except it
   * uses the local depth of the bucket located at bucket_idx
   *
   * @param bucket_idx the index to use for looking up local depth
   * @return mask of local 1's and the rest 0's (with 1's from LSB upwards)
   */
  auto GetLocalDepthMask(uint32_t bucket_idx) -> uint32_t;

  auto GetGlobalDepth() -> uint32_t;

  void IncrGlobalDepth();

  void DecrGlobalDepth();

  auto CanShrink() -> bool;

  auto Size() -> uint32_t;

  auto GetLocalDepth(uint32_t bucket_idx) -> uint32_t;

  void SetLocalDepth(uint32_t bucket_idx, uint8_t local_depth);

  void IncrLocalDepth(uint32_t bucket_idx);

  void DecrLocalDepth(uint32_t bucket_idx);

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

  void PrintDirectory();

 private:
  page_id_t page_id_;
  lsn_t lsn_;
  uint32_t global_depth_{0};
  uint8_t local_depths_[DIRECTORY_ARRAY_SIZE];
  page_id_t bucket_page_ids_[DIRECTORY_ARRAY_SIZE];
};

}  // namespace bustub
