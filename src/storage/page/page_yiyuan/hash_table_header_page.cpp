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
page_id_t HashTableHeaderPage::GetBlockPageId(size_t index) { return 0; }

page_id_t HashTableHeaderPage::GetPageId() const { return 0; }

void HashTableHeaderPage::SetPageId(bustub::page_id_t page_id) {}

lsn_t HashTableHeaderPage::GetLSN() const { return 0; }

void HashTableHeaderPage::SetLSN(lsn_t lsn) {}

void HashTableHeaderPage::AddBlockPageId(page_id_t page_id) {}

size_t HashTableHeaderPage::NumBlocks() { return 0; }

void HashTableHeaderPage::SetSize(size_t size) {}

size_t HashTableHeaderPage::GetSize() const { return 0; }

}  // namespace bustub
