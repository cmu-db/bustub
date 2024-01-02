//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_iterator.cpp
//
// Identification: src/storage/table/table_iterator.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <optional>

#include "common/config.h"
#include "common/exception.h"
#include "concurrency/transaction.h"
#include "storage/table/table_heap.h"

namespace bustub {

TableIterator::TableIterator(TableHeap *table_heap, RID rid, RID stop_at_rid)
    : table_heap_(table_heap), rid_(rid), stop_at_rid_(stop_at_rid) {
  // If the rid doesn't correspond to a tuple (i.e., the table has just been initialized), then
  // we set rid_ to invalid.
  if (rid.GetPageId() == INVALID_PAGE_ID) {
    rid_ = RID{INVALID_PAGE_ID, 0};
  } else {
    auto page_guard = table_heap_->bpm_->FetchPageRead(rid_.GetPageId());
    auto page = page_guard.As<TablePage>();
    if (rid_.GetSlotNum() >= page->GetNumTuples()) {
      rid_ = RID{INVALID_PAGE_ID, 0};
    }
  }
}

auto TableIterator::GetTuple() -> std::pair<TupleMeta, Tuple> { return table_heap_->GetTuple(rid_); }

auto TableIterator::GetRID() -> RID { return rid_; }

auto TableIterator::IsEnd() -> bool { return rid_.GetPageId() == INVALID_PAGE_ID; }

auto TableIterator::operator++() -> TableIterator & {
  auto page_guard = table_heap_->bpm_->FetchPageRead(rid_.GetPageId());
  auto page = page_guard.As<TablePage>();
  auto next_tuple_id = rid_.GetSlotNum() + 1;

  if (stop_at_rid_.GetPageId() != INVALID_PAGE_ID) {
    BUSTUB_ASSERT(
        /* case 1: cursor before the page of the stop tuple */ rid_.GetPageId() < stop_at_rid_.GetPageId() ||
            /* case 2: cursor at the page before the tuple */
            (rid_.GetPageId() == stop_at_rid_.GetPageId() && next_tuple_id <= stop_at_rid_.GetSlotNum()),
        "iterate out of bound");
  }

  rid_ = RID{rid_.GetPageId(), next_tuple_id};

  if (rid_ == stop_at_rid_) {
    rid_ = RID{INVALID_PAGE_ID, 0};
  } else if (next_tuple_id < page->GetNumTuples()) {
    // that's fine
  } else {
    auto next_page_id = page->GetNextPageId();
    // if next page is invalid, RID is set to invalid page; otherwise, it's the first tuple in that page.
    rid_ = RID{next_page_id, 0};
  }

  page_guard.Drop();

  return *this;
}

}  // namespace bustub
