//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// tmp_tuple.h
//
// Identification: src/include/storage/table/tmp_tuple.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "common/config.h"

namespace bustub {

// Document this class! What does it represent?

class TmpTuple {
 public:
  TmpTuple(page_id_t page_id, size_t offset) : page_id_(page_id), offset_(offset) {}

  inline auto operator==(const TmpTuple &rhs) const -> bool {
    return page_id_ == rhs.page_id_ && offset_ == rhs.offset_;
  }

  auto GetPageId() const -> page_id_t { return page_id_; }
  auto GetOffset() const -> size_t { return offset_; }

 private:
  page_id_t page_id_;
  size_t offset_;
};

}  // namespace bustub
