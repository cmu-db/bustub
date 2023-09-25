//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_header_page.h
//
// Identification: src/include/storage/page/extendible_htable_header_page.h
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

/**
 * Header page format:
 *  ---------------------------------------------------
 * | DirectoryPageIds(2048) | MaxDepth (4) | Free(2044)
 *  ---------------------------------------------------
 */

#pragma once

#include <cstdlib>
#include "common/config.h"
#include "common/macros.h"

namespace bustub {

static constexpr uint64_t HTABLE_HEADER_PAGE_METADATA_SIZE = sizeof(uint32_t);
static constexpr uint64_t HTABLE_HEADER_MAX_DEPTH = 9;
static constexpr uint64_t HTABLE_HEADER_ARRAY_SIZE = 1 << HTABLE_HEADER_MAX_DEPTH;

class ExtendibleHTableHeaderPage {
 public:
  // Delete all constructor / destructor to ensure memory safety
  ExtendibleHTableHeaderPage() = delete;
  DISALLOW_COPY_AND_MOVE(ExtendibleHTableHeaderPage);

  /**
   * After creating a new header page from buffer pool, must call initialize
   * method to set default values
   * @param max_depth Max depth in the header page
   */
  void Init(uint32_t max_depth = HTABLE_HEADER_MAX_DEPTH);

  /**
   * Get the directory index that the key is hashed to
   *
   * @param hash the hash of the key
   * @return directory index the key is hashed to
   */
  auto HashToDirectoryIndex(uint32_t hash) const -> uint32_t;

  /**
   * Get the directory page id at an index
   *
   * @param directory_idx index in the directory page id array
   * @return directory page_id at index
   */
  auto GetDirectoryPageId(uint32_t directory_idx) const -> uint32_t;

  /**
   * @brief Set the directory page id at an index
   *
   * @param directory_idx index in the directory page id array
   * @param directory_page_id page id of the directory
   */
  void SetDirectoryPageId(uint32_t directory_idx, page_id_t directory_page_id);

  /**
   * @brief Get the maximum number of directory page ids the header page could handle
   */
  auto MaxSize() const -> uint32_t;

  /**
   * Prints the header's occupancy information
   */
  void PrintHeader() const;

 private:
  page_id_t directory_page_ids_[HTABLE_HEADER_ARRAY_SIZE];
  uint32_t max_depth_;
};

static_assert(sizeof(page_id_t) == 4);

static_assert(sizeof(ExtendibleHTableHeaderPage) ==
              sizeof(page_id_t) * HTABLE_HEADER_ARRAY_SIZE + HTABLE_HEADER_PAGE_METADATA_SIZE);

static_assert(sizeof(ExtendibleHTableHeaderPage) <= BUSTUB_PAGE_SIZE);

}  // namespace bustub
