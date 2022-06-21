//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_header_page.cpp
//
// Identification: src/storage/page/hash_table_header_page.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/hash_table_header_page.h"

namespace bustub {
auto HashTableHeaderPage::GetBlockPageId(size_t index) -> page_id_t { return 0; }

auto HashTableHeaderPage::GetPageId() const -> page_id_t { return 0; }

void HashTableHeaderPage::SetPageId(bustub::page_id_t page_id) {}

auto HashTableHeaderPage::GetLSN() const -> lsn_t { return 0; }

void HashTableHeaderPage::SetLSN(lsn_t lsn) {}

void HashTableHeaderPage::AddBlockPageId(page_id_t page_id) {}

auto HashTableHeaderPage::NumBlocks() -> size_t { return 0; }

void HashTableHeaderPage::SetSize(size_t size) {}

auto HashTableHeaderPage::GetSize() const -> size_t { return 0; }

}  // namespace bustub
