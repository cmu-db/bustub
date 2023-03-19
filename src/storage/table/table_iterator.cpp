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

TableIterator::TableIterator(TableHeap *table_heap, RID rid) : table_heap_(table_heap), rid_(rid) {
  // If the rid doesn't correspond to a tuple (i.e., the table has just been initialized), then
  // we set rid_ to invalid.
  auto page_guard = table_heap_->bpm_->FetchPageRead(rid_.GetPageId());
  auto page = page_guard.As<TablePage>();
  if (rid_.GetSlotNum() >= page->GetNumTuples()) {
    rid_ = RID{INVALID_PAGE_ID, 0};
  }
}

auto TableIterator::GetTuple() -> std::pair<TupleMeta, Tuple> { return table_heap_->GetTuple(rid_); }

auto TableIterator::GetRID() -> RID { return rid_; }

auto TableIterator::IsEnd() -> bool { return rid_.GetPageId() == INVALID_PAGE_ID; }

auto TableIterator::operator++() -> TableIterator & {
  auto page_guard = table_heap_->bpm_->FetchPageRead(rid_.GetPageId());
  auto page = page_guard.As<TablePage>();
  auto next_tuple_id = rid_.GetSlotNum() + 1;
  if (next_tuple_id < page->GetNumTuples()) {
    rid_ = RID{rid_.GetPageId(), next_tuple_id};
  } else {
    auto next_page_id = page->GetNextPageId();
    // if next page is invalid, RID is set to invalid page; otherwise, it's the first tuple in that page.
    rid_ = RID{next_page_id, 0};
  }
  return *this;
}

}  // namespace bustub
