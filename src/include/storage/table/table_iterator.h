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
  TableIterator(TableHeap *table_heap, RID rid, Transaction *txn);

  ~TableIterator() { delete tuple_; }

  inline bool operator==(const TableIterator &itr) const { return tuple_->rid_.Get() == itr.tuple_->rid_.Get(); }

  inline bool operator!=(const TableIterator &itr) const { return !(*this == itr); }

  const Tuple &operator*();

  Tuple *operator->();

  TableIterator &operator++();

  TableIterator operator++(int);

 private:
  TableHeap *table_heap_;
  Tuple *tuple_;
  Transaction *txn_;
};

}  // namespace bustub
