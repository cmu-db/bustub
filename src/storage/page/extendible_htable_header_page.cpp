//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_header_page.cpp
//
// Identification: src/storage/page/extendible_htable_header_page.cpp
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/extendible_htable_header_page.h"

#include "common/exception.h"
#include "common/logger.h"

namespace bustub {

void ExtendibleHTableHeaderPage::Init(uint32_t max_depth) {
  throw NotImplementedException("ExtendibleHTableHeaderPage is not implemented");
}

auto ExtendibleHTableHeaderPage::HashToDirectoryIndex(uint32_t hash) const -> uint32_t { return 0; }

auto ExtendibleHTableHeaderPage::GetDirectoryPageId(uint32_t directory_idx) const -> uint32_t { return 0; }

void ExtendibleHTableHeaderPage::SetDirectoryPageId(uint32_t directory_idx, page_id_t directory_page_id) {
  throw NotImplementedException("ExtendibleHTableHeaderPage is not implemented");
}

auto ExtendibleHTableHeaderPage::MaxSize() const -> uint32_t { return 0; }

void ExtendibleHTableHeaderPage::PrintHeader() const {
  LOG_DEBUG("======== HEADER (max_depth_: %u) ========", max_depth_);
  LOG_DEBUG("| directory_idx | page_id |");
  for (uint32_t idx = 0; idx < static_cast<uint32_t>(1 << max_depth_); idx++) {
    LOG_DEBUG("|    %u    |    %u    |", idx, directory_page_ids_[idx]);
  }
  LOG_DEBUG("======== END HEADER ========");
}

}  // namespace bustub
