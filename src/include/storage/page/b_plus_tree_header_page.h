//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree_header_page.h
//
// Identification: src/include/storage/page/b_plus_tree_header_page.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "common/config.h"

namespace bustub {

/**
 * The header page is just used to retrieve the root page,
 * preventing potential race condition under concurrent environment.
 */
class BPlusTreeHeaderPage {
 public:
  // Delete all constructor / destructor to ensure memory safety
  BPlusTreeHeaderPage() = delete;
  BPlusTreeHeaderPage(const BPlusTreeHeaderPage &other) = delete;

  page_id_t root_page_id_;
};

}  // namespace bustub
