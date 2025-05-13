//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_page.h
//
// Identification: src/include/storage/page/table_page.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstring>
#include <optional>
#include <tuple>
#include <utility>

#include "common/config.h"
#include "common/rid.h"
#include "concurrency/lock_manager.h"
#include "recovery/log_manager.h"
#include "storage/page/page.h"
#include "storage/table/table_heap.h"
#include "storage/table/tuple.h"

namespace bustub {

static constexpr uint64_t TABLE_PAGE_HEADER_SIZE = 8;

/**
 * Slotted page format:
 *  ---------------------------------------------------------
 *  | HEADER | ... FREE SPACE ... | ... INSERTED TUPLES ... |
 *  ---------------------------------------------------------
 *                                ^
 *                                free space pointer
 *
 *  Header format (size in bytes):
 *  ----------------------------------------------------------------------------
 *  | NextPageId (4)| NumTuples(2) | NumDeletedTuples(2) |
 *  ----------------------------------------------------------------------------
 *  ----------------------------------------------------------------
 *  | Tuple_1 offset+size (4) | Tuple_2 offset+size (4) | ... |
 *  ----------------------------------------------------------------
 *
 * Tuple format:
 * | meta | data |
 */

class TablePage {
 public:
  void Init();

  /** @return number of tuples in this page */
  auto GetNumTuples() const -> uint32_t { return num_tuples_; }

  /** @return the page ID of the next table page */
  auto GetNextPageId() const -> page_id_t { return next_page_id_; }

  /** Set the page id of the next page in the table. */
  void SetNextPageId(page_id_t next_page_id) { next_page_id_ = next_page_id; }

  auto GetNextTupleOffset(const TupleMeta &meta, const Tuple &tuple) const -> std::optional<uint16_t>;

  auto InsertTuple(const TupleMeta &meta, const Tuple &tuple) -> std::optional<uint16_t>;

  void UpdateTupleMeta(const TupleMeta &meta, const RID &rid);

  auto GetTuple(const RID &rid) const -> std::pair<TupleMeta, Tuple>;

  auto GetTupleMeta(const RID &rid) const -> TupleMeta;

  void UpdateTupleInPlaceUnsafe(const TupleMeta &meta, const Tuple &tuple, RID rid);

  static_assert(sizeof(page_id_t) == 4);

 private:
  using TupleInfo = std::tuple<uint16_t, uint16_t, TupleMeta>;
  char page_start_[0];
  page_id_t next_page_id_;
  uint16_t num_tuples_;
  uint16_t num_deleted_tuples_;
  TupleInfo tuple_info_[0];

  static constexpr size_t TUPLE_INFO_SIZE = 24;
  static_assert(sizeof(TupleInfo) == TUPLE_INFO_SIZE);
};

static_assert(sizeof(TablePage) == TABLE_PAGE_HEADER_SIZE);

}  // namespace bustub
