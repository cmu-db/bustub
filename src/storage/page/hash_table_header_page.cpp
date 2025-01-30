//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_header_page.cpp
//
// Identification: src/storage/page/hash_table_header_page.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/hash_table_header_page.h"

namespace bustub {

/**
 * Returns the page_id of the index-th block
 *
 * @param index the index of the block
 * @return the page_id for the block.
 */
auto HashTableHeaderPage::GetBlockPageId(size_t index) -> page_id_t { return 0; }

/**
 * @return the page ID of this page
 */
auto HashTableHeaderPage::GetPageId() const -> page_id_t { return 0; }

/**
 * Sets the page ID of this page
 *
 * @param page_id the page id for the page id field to be set to
 */
void HashTableHeaderPage::SetPageId(bustub::page_id_t page_id) {}

/**
 * @return the lsn of this page
 */
auto HashTableHeaderPage::GetLSN() const -> lsn_t { return 0; }

/**
 * Sets the LSN of this page
 *
 * @param lsn the log sequence number for the lsn field to be set to
 */
void HashTableHeaderPage::SetLSN(lsn_t lsn) {}

/**
 * Adds a block page_id to the end of header page
 *
 * @param page_id page_id to be added
 */
void HashTableHeaderPage::AddBlockPageId(page_id_t page_id) {}

/**
 * @return the number of blocks currently stored in the header page
 */
auto HashTableHeaderPage::NumBlocks() -> size_t { return 0; }

/**
 * Sets the size field of the hash table to size
 *
 * @param size the size for the size field to be set to
 */
void HashTableHeaderPage::SetSize(size_t size) {}

/**
 * @return the number of buckets in the hash table;
 */
auto HashTableHeaderPage::GetSize() const -> size_t { return 0; }

}  // namespace bustub
