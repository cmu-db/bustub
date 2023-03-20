//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_iterator.h
//
// Identification: src/include/storage/table/table_iterator.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <memory>
#include <utility>

#include "common/macros.h"
#include "common/rid.h"
#include "concurrency/transaction.h"
#include "storage/table/tuple.h"

namespace bustub {

class TableHeap;

/**
 * TableIterator enables the sequential scan of a TableHeap.
 */
class TableIterator {
  friend class Cursor;

 public:
  DISALLOW_COPY(TableIterator);

  TableIterator(TableHeap *table_heap, RID rid, RID stop_at_rid);
  TableIterator(TableIterator &&) = default;

  ~TableIterator() = default;

  auto GetTuple() -> std::pair<TupleMeta, Tuple>;

  auto GetRID() -> RID;

  auto IsEnd() -> bool;

  auto operator++() -> TableIterator &;

 private:
  TableHeap *table_heap_;
  RID rid_;

  // When creating table iterator, we will record the maximum RID that we should scan.
  // Otherwise we will have dead loops when updating while scanning. (In project 4, update should be implemented as
  // deletion + insertion.)
  RID stop_at_rid_;
};

}  // namespace bustub
