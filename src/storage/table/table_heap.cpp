//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_heap.cpp
//
// Identification: src/storage/table/table_heap.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>
#include <mutex>  // NOLINT
#include <utility>

#include "common/config.h"
#include "common/exception.h"
#include "common/logger.h"
#include "common/macros.h"
#include "fmt/format.h"
#include "storage/page/page_guard.h"
#include "storage/page/table_page.h"
#include "storage/table/table_heap.h"

namespace bustub {

TableHeap::TableHeap(BufferPoolManager *bpm) : bpm_(bpm) {
  // Initialize the first table page.
  auto guard = bpm->NewPageGuarded(&first_page_id_);
  last_page_id_ = first_page_id_;
  auto first_page = guard.AsMut<TablePage>();
  BUSTUB_ASSERT(first_page != nullptr,
                "Couldn't create a page for the table heap. Have you completed the buffer pool manager project?");
  first_page->Init();
}

auto TableHeap::InsertTuple(const TupleMeta &meta, const Tuple &tuple) -> std::optional<RID> {
  std::unique_lock<std::mutex> guard(latch_);
  auto page_guard = bpm_->FetchPageWrite(last_page_id_);
  while (true) {
    auto page = page_guard.AsMut<TablePage>();
    if (page->GetNextTupleOffset(meta, tuple) != std::nullopt) {
      break;
    }

    // if there's no tuple in the page, and we can't insert the tuple, then this tuple is too large.
    BUSTUB_ENSURE(page->GetNumTuples() != 0, "tuple is too large, cannot insert");

    page_id_t next_page_id = INVALID_PAGE_ID;
    auto npg = bpm_->NewPage(&next_page_id);
    BUSTUB_ENSURE(next_page_id != INVALID_PAGE_ID, "cannot allocate page");

    // Don't do lock crabbing here: TSAN reports, also as last_page_id_ is only updated
    // later, this page won't be accessed.
    page->SetNextPageId(next_page_id);
    page_guard.Drop();

    npg->WLatch();
    auto next_page_guard = WritePageGuard{bpm_, npg};
    auto next_page = next_page_guard.AsMut<TablePage>();
    next_page->Init();

    last_page_id_ = next_page_id;
    page_guard = std::move(next_page_guard);
  }
  auto last_page_id = last_page_id_;
  guard.unlock();

  auto page = page_guard.AsMut<TablePage>();
  auto slot_id = *page->InsertTuple(meta, tuple);

  page_guard.Drop();

  return RID(last_page_id, slot_id);
}

void TableHeap::UpdateTupleMeta(const TupleMeta &meta, RID rid) {
  auto page_guard = bpm_->FetchPageWrite(rid.GetPageId());
  auto page = page_guard.AsMut<TablePage>();
  page->UpdateTupleMeta(meta, rid);
}

auto TableHeap::GetTuple(RID rid) -> std::pair<TupleMeta, Tuple> {
  auto page_guard = bpm_->FetchPageRead(rid.GetPageId());
  auto page = page_guard.As<TablePage>();
  auto [meta, tuple] = page->GetTuple(rid);
  tuple.rid_ = rid;
  return std::make_pair(meta, std::move(tuple));
}

auto TableHeap::GetTupleMeta(RID rid) -> TupleMeta {
  auto page_guard = bpm_->FetchPageRead(rid.GetPageId());
  auto page = page_guard.As<TablePage>();
  return page->GetTupleMeta(rid);
}

auto TableHeap::MakeIterator() -> TableIterator {
  std::unique_lock<std::mutex> guard(latch_);
  auto last_page_id = last_page_id_;
  guard.unlock();

  auto page_guard = bpm_->FetchPageRead(last_page_id);
  auto page = page_guard.As<TablePage>();
  return {this, {first_page_id_, 0}, {last_page_id, page->GetNumTuples()}};
}

void TableHeap::UpdateTupleInPlaceUnsafe(const TupleMeta &meta, const Tuple &tuple, RID rid) {
  auto page_guard = bpm_->FetchPageWrite(rid.GetPageId());
  auto page = page_guard.AsMut<TablePage>();
  page->UpdateTupleInPlaceUnsafe(meta, tuple, rid);
}

}  // namespace bustub
