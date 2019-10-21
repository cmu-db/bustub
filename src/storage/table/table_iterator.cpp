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

#include "storage/table/table_heap.h"

namespace bustub {

TableIterator::TableIterator(TableHeap *table_heap, RID rid, Transaction *txn)
    : table_heap_(table_heap), tuple_(new Tuple(rid)), txn_(txn) {
  if (rid.GetPageId() != INVALID_PAGE_ID) {
    table_heap_->GetTuple(tuple_->rid_, tuple_, txn_);
  }
}

const Tuple &TableIterator::operator*() {
  assert(*this != table_heap_->End());
  return *tuple_;
}

Tuple *TableIterator::operator->() {
  assert(*this != table_heap_->End());
  return tuple_;
}

TableIterator &TableIterator::operator++() {
  BufferPoolManager *buffer_pool_manager = table_heap_->buffer_pool_manager_;
  auto cur_page = static_cast<TablePage *>(buffer_pool_manager->FetchPage(tuple_->rid_.GetPageId()));
  cur_page->RLatch();
  assert(cur_page != nullptr);  // all pages are pinned

  RID next_tuple_rid;
  if (!cur_page->GetNextTupleRid(tuple_->rid_,
                                 &next_tuple_rid)) {  // end of this page
    while (cur_page->GetNextPageId() != INVALID_PAGE_ID) {
      auto next_page = static_cast<TablePage *>(buffer_pool_manager->FetchPage(cur_page->GetNextPageId()));
      cur_page->RUnlatch();
      buffer_pool_manager->UnpinPage(cur_page->GetTablePageId(), false);
      cur_page = next_page;
      cur_page->RLatch();
      if (cur_page->GetFirstTupleRid(&next_tuple_rid)) {
        break;
      }
    }
  }
  tuple_->rid_ = next_tuple_rid;

  if (*this != table_heap_->End()) {
    table_heap_->GetTuple(tuple_->rid_, tuple_, txn_);
  }
  // release until copy the tuple
  cur_page->RUnlatch();
  buffer_pool_manager->UnpinPage(cur_page->GetTablePageId(), false);
  return *this;
}

TableIterator TableIterator::operator++(int) {
  TableIterator clone(*this);
  ++(*this);
  return clone;
}

}  // namespace bustub
