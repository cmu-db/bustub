//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_header_page.h
//
// Identification: src/include/storage/page/hash_table_header_page.h
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
 * Header Page for linear probing hash table.
 *
 * Header format (size in byte, 16 bytes in total):
 * -------------------------------------------------------------
 * | LSN (4) | Size (4) | PageId(4) | NextBlockIndex(4)
 * -------------------------------------------------------------
 */
class HashTableHeaderPage {
 public:
  auto GetSize() const -> size_t;

  void SetSize(size_t size);

  auto GetPageId() const -> page_id_t;

  void SetPageId(page_id_t page_id);

  auto GetLSN() const -> lsn_t;

  void SetLSN(lsn_t lsn);

  void AddBlockPageId(page_id_t page_id);

  auto GetBlockPageId(size_t index) -> page_id_t;

  auto NumBlocks() -> size_t;

 private:
  __attribute__((unused)) lsn_t lsn_;
  __attribute__((unused)) size_t size_;
  __attribute__((unused)) page_id_t page_id_;
  __attribute__((unused)) size_t next_ind_;
  // Flexible array member for page data.
  __attribute__((unused)) page_id_t block_page_ids_[1];
};

}  // namespace bustub
