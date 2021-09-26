//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_header_page.h
//
// Identification: src/include/storage/page/hash_table_header_page.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
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
 */
class HashTableDirectoryPage {
 public:
  /**
   * @return the page ID of this page
   */
  page_id_t GetPageId() const;

  /**
   * Sets the page ID of this page
   *
   * @param page_id the page id to which to set the page_id_ field
   */
  void SetPageId(page_id_t page_id);

  /**
   * @return the lsn of this page
   */
  lsn_t GetLSN() const;

  /**
   * Sets the LSN of this page
   *
   * @param lsn the log sequence number to which to set the lsn field
   */
  void SetLSN(lsn_t lsn);

  /**
   * Get the global depth of the hash table directory
   *
   * @return the global depth of the directory
   */
  uint32_t GetGlobalDepth();

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
  bool CanShrink();

  /**
   * Lookup a bucket page using a directory index
   *
   * @param bucket_idx the index in the directory to lookup
   * @return bucket page_id corresponding to bucket_idx
   */
  page_id_t GetBucketPageId(uint32_t bucket_idx);

  /**
   * @return the current directory size
   */
  uint32_t Size();

  /**
   * Updates the directory index using a bucket index and page_id
   *
   * @param bucket_idx directory index at which to insert page_id
   * @param bucket_page_id page_id to insert
   */
  void SetBucketPageId(uint32_t bucket_idx, page_id_t bucket_page_id);

  /**
   * @param bucket_idx
   */
  uint32_t GetLocalDepth(uint32_t bucket_idx);

  /**
   * Set the local depth of the bucket at bucket_idx to local_depth
   * @param bucket_idx bucket index to update
   * @param local_depth new local depth to set
   */
  void SetLocalDepth(uint32_t bucket_idx, uint8_t local_depth);

  /**
   * Increment the local depth of the bucket at bucket_idx
   */
  void IncrLocalDepth(uint32_t bucket_idx);

  /**
   * Decrement the local depth of the bucket at bucket_idx
   */
  void DecrLocalDepth(uint32_t bucket_idx);

  /**
   * @return the high bit corresponding to the bucket's local depth
   */
  uint32_t GetLocalHighBit(uint32_t bucket_idx);

 private:
  __attribute__((unused)) lsn_t lsn_;
  __attribute__((unused)) page_id_t page_id_;
  __attribute__((unused)) uint32_t global_depth_{0};
  __attribute__((unused)) uint8_t local_depths_[DIRECTORY_ARRAY_SIZE];
  __attribute__((unused)) page_id_t bucket_page_ids_[DIRECTORY_ARRAY_SIZE];
};

}  // namespace bustub
