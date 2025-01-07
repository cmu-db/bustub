//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_header_page.cpp
//
// Identification: src/storage/page/extendible_htable_header_page.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/extendible_htable_header_page.h"

#include "common/exception.h"

namespace bustub {

/**
 * After creating a new header page from buffer pool, must call initialize
 * method to set default values
 * @param max_depth Max depth in the header page
 */
void ExtendibleHTableHeaderPage::Init(uint32_t max_depth) {
  throw NotImplementedException("ExtendibleHTableHeaderPage is not implemented");
}

/**
 * Get the directory index that the key is hashed to
 *
 * @param hash the hash of the key
 * @return directory index the key is hashed to
 */
auto ExtendibleHTableHeaderPage::HashToDirectoryIndex(uint32_t hash) const -> uint32_t { return 0; }

/**
 * Get the directory page id at an index
 *
 * @param directory_idx index in the directory page id array
 * @return directory page_id at index
 */
auto ExtendibleHTableHeaderPage::GetDirectoryPageId(uint32_t directory_idx) const -> page_id_t { return 0; }

/**
 * @brief Set the directory page id at an index
 *
 * @param directory_idx index in the directory page id array
 * @param directory_page_id page id of the directory
 */
void ExtendibleHTableHeaderPage::SetDirectoryPageId(uint32_t directory_idx, page_id_t directory_page_id) {
  throw NotImplementedException("ExtendibleHTableHeaderPage is not implemented");
}

/**
 * @brief Get the maximum number of directory page ids the header page could handle
 */
auto ExtendibleHTableHeaderPage::MaxSize() const -> uint32_t { return 0; }

}  // namespace bustub
