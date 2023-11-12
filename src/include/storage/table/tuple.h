//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// tuple.h
//
// Identification: src/include/storage/table/tuple.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

#include "catalog/schema.h"
#include "common/config.h"
#include "common/rid.h"
#include "type/value.h"

namespace bustub {

using timestamp_t = int64_t;
const timestamp_t INVALID_TS = -1;

static constexpr size_t TUPLE_META_SIZE = 16;

struct TupleMeta {
  /** the ts / txn_id of this tuple. In project 3, simply set it to 0. */
  timestamp_t ts_;
  /** marks whether this tuple is marked removed from table heap. */
  bool is_deleted_;

  friend auto operator==(const TupleMeta &a, const TupleMeta &b) {
    return a.ts_ == b.ts_ && a.is_deleted_ == b.is_deleted_;
  }

  friend auto operator!=(const TupleMeta &a, const TupleMeta &b) { return !(a == b); }
};

static_assert(sizeof(TupleMeta) == TUPLE_META_SIZE);

/**
 * Tuple format:
 * ---------------------------------------------------------------------
 * | FIXED-SIZE or VARIED-SIZED OFFSET | PAYLOAD OF VARIED-SIZED FIELD |
 * ---------------------------------------------------------------------
 */
class Tuple {
  friend class TablePage;
  friend class TableHeap;
  friend class TableIterator;

 public:
  // Default constructor (to create a dummy tuple)
  Tuple() = default;

  // constructor for table heap tuple
  explicit Tuple(RID rid) : rid_(rid) {}

  static auto Empty() -> Tuple { return Tuple{RID{INVALID_PAGE_ID, 0}}; }

  // constructor for creating a new tuple based on input value
  Tuple(std::vector<Value> values, const Schema *schema);

  Tuple(const Tuple &other) = default;

  // move constructor
  Tuple(Tuple &&other) noexcept = default;

  // assign operator, deep copy
  auto operator=(const Tuple &other) -> Tuple & = default;

  // move assignment
  auto operator=(Tuple &&other) noexcept -> Tuple & = default;

  // serialize tuple data
  void SerializeTo(char *storage) const;

  // deserialize tuple data(deep copy)
  void DeserializeFrom(const char *storage);

  // return RID of current tuple
  inline auto GetRid() const -> RID { return rid_; }

  // return RID of current tuple
  inline auto SetRid(RID rid) { rid_ = rid; }

  // Get the address of this tuple in the table's backing store
  inline auto GetData() const -> const char * { return data_.data(); }

  // Get length of the tuple, including varchar length
  inline auto GetLength() const -> uint32_t { return data_.size(); }

  // Get the value of a specified column (const)
  // checks the schema to see how to return the Value.
  auto GetValue(const Schema *schema, uint32_t column_idx) const -> Value;

  // Generates a key tuple given schemas and attributes
  auto KeyFromTuple(const Schema &schema, const Schema &key_schema, const std::vector<uint32_t> &key_attrs) -> Tuple;

  // Is the column value null ?
  inline auto IsNull(const Schema *schema, uint32_t column_idx) const -> bool {
    Value value = GetValue(schema, column_idx);
    return value.IsNull();
  }

  auto ToString(const Schema *schema) const -> std::string;

  friend inline auto IsTupleContentEqual(const Tuple &a, const Tuple &b) { return a.data_ == b.data_; }

 private:
  // Get the starting storage address of specific column
  auto GetDataPtr(const Schema *schema, uint32_t column_idx) const -> const char *;

  RID rid_{};  // if pointing to the table heap, the rid is valid
  std::vector<char> data_;
};

}  // namespace bustub
