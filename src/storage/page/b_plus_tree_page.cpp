//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/page/b_plus_tree_page.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/b_plus_tree_page.h"

namespace bustub {

/*
 * Helper methods to get/set page type
 * Page type enum class is defined in b_plus_tree_page.h
 */
auto BPlusTreePage::IsLeafPage() const -> bool { return false; }
auto BPlusTreePage::IsRootPage() const -> bool { return false; }
void BPlusTreePage::SetPageType(IndexPageType page_type) {}

/*
 * Helper methods to get/set size (number of key/value pairs stored in that
 * page)
 */
auto BPlusTreePage::GetSize() const -> int { return 0; }
void BPlusTreePage::SetSize(int size) {}
void BPlusTreePage::IncreaseSize(int amount) {}

/*
 * Helper methods to get/set max size (capacity) of the page
 */
auto BPlusTreePage::GetMaxSize() const -> int { return 0; }
void BPlusTreePage::SetMaxSize(int size) {}

/*
 * Helper method to get min page size
 * Generally, min page size == max page size / 2
 */
auto BPlusTreePage::GetMinSize() const -> int { return 0; }

/*
 * Helper methods to get/set parent page id
 */
auto BPlusTreePage::GetParentPageId() const -> page_id_t { return 0; }
void BPlusTreePage::SetParentPageId(page_id_t parent_page_id) {}

/*
 * Helper methods to get/set self page id
 */
auto BPlusTreePage::GetPageId() const -> page_id_t { return 0; }
void BPlusTreePage::SetPageId(page_id_t page_id) {}

/*
 * Helper methods to set lsn
 */
void BPlusTreePage::SetLSN(lsn_t lsn) { lsn_ = lsn; }

}  // namespace bustub
