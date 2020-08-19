//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// log_record.h
//
// Identification: src/include/recovery/log_record.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cassert>
#include <string>

#include "common/config.h"
#include "storage/table/tuple.h"

namespace bustub {
/** The type of the log record. */
enum class LogRecordType {
  INVALID = 0,
  INSERT,
  MARKDELETE,
  APPLYDELETE,
  ROLLBACKDELETE,
  UPDATE,
  BEGIN,
  COMMIT,
  ABORT,
  /** Creating a new page in the table heap. */
  NEWPAGE,
};

/**
 * For every write operation on the table page, you should write ahead a corresponding log record.
 *
 * For EACH log record, HEADER is like (5 fields in common, 20 bytes in total).
 *---------------------------------------------
 * | size | LSN | transID | prevLSN | LogType |
 *---------------------------------------------
 * For insert type log record
 *---------------------------------------------------------------
 * | HEADER | tuple_rid | tuple_size | tuple_data(char[] array) |
 *---------------------------------------------------------------
 * For delete type (including markdelete, rollbackdelete, applydelete)
 *----------------------------------------------------------------
 * | HEADER | tuple_rid | tuple_size | tuple_data(char[] array) |
 *---------------------------------------------------------------
 * For update type log record
 *-----------------------------------------------------------------------------------
 * | HEADER | tuple_rid | tuple_size | old_tuple_data | tuple_size | new_tuple_data |
 *-----------------------------------------------------------------------------------
 * For new page type log record
 *--------------------------
 * | HEADER | prev_page_id |
 *--------------------------
 */
class LogRecord {
  friend class LogManager;
  friend class LogRecovery;

 public:
  LogRecord() = default;

  // constructor for Transaction type(BEGIN/COMMIT/ABORT)
  LogRecord(txn_id_t txn_id, lsn_t prev_lsn, LogRecordType log_record_type)
      : size_(HEADER_SIZE), txn_id_(txn_id), prev_lsn_(prev_lsn), log_record_type_(log_record_type) {}

  // constructor for INSERT/DELETE type
  LogRecord(txn_id_t txn_id, lsn_t prev_lsn, LogRecordType log_record_type, const RID &rid, const Tuple &tuple)
      : txn_id_(txn_id), prev_lsn_(prev_lsn), log_record_type_(log_record_type) {
    if (log_record_type == LogRecordType::INSERT) {
      insert_rid_ = rid;
      insert_tuple_ = tuple;
    } else {
      assert(log_record_type == LogRecordType::APPLYDELETE || log_record_type == LogRecordType::MARKDELETE ||
             log_record_type == LogRecordType::ROLLBACKDELETE);
      delete_rid_ = rid;
      delete_tuple_ = tuple;
    }
    // calculate log record size
    size_ = HEADER_SIZE + sizeof(RID) + sizeof(int32_t) + tuple.GetLength();
  }

  // constructor for UPDATE type
  LogRecord(txn_id_t txn_id, lsn_t prev_lsn, LogRecordType log_record_type, const RID &update_rid,
            const Tuple &old_tuple, const Tuple &new_tuple)
      : txn_id_(txn_id),
        prev_lsn_(prev_lsn),
        log_record_type_(log_record_type),
        update_rid_(update_rid),
        old_tuple_(old_tuple),
        new_tuple_(new_tuple) {
    // calculate log record size
    size_ = HEADER_SIZE + sizeof(RID) + old_tuple.GetLength() + new_tuple.GetLength() + 2 * sizeof(int32_t);
  }

  // constructor for NEWPAGE type
  LogRecord(txn_id_t txn_id, lsn_t prev_lsn, LogRecordType log_record_type, page_id_t prev_page_id, page_id_t page_id)
      : size_(HEADER_SIZE),
        txn_id_(txn_id),
        prev_lsn_(prev_lsn),
        log_record_type_(log_record_type),
        prev_page_id_(prev_page_id),
        page_id_(page_id) {
    // calculate log record size, header size + sizeof(prev_page_id) + sizeof(page_id)
    size_ = HEADER_SIZE + sizeof(page_id_t) * 2;
  }

  ~LogRecord() = default;

  inline Tuple &GetDeleteTuple() { return delete_tuple_; }

  inline RID &GetDeleteRID() { return delete_rid_; }

  inline Tuple &GetInsertTuple() { return insert_tuple_; }

  inline RID &GetInsertRID() { return insert_rid_; }

  inline Tuple &GetOriginalTuple() { return old_tuple_; }

  inline Tuple &GetUpdateTuple() { return new_tuple_; }

  inline RID &GetUpdateRID() { return update_rid_; }

  inline page_id_t GetNewPageRecord() { return prev_page_id_; }

  inline int32_t GetSize() { return size_; }

  inline lsn_t GetLSN() { return lsn_; }

  inline txn_id_t GetTxnId() { return txn_id_; }

  inline lsn_t GetPrevLSN() { return prev_lsn_; }

  inline LogRecordType &GetLogRecordType() { return log_record_type_; }

  // For debug purpose
  inline std::string ToString() const {
    std::ostringstream os;
    os << "Log["
       << "size:" << size_ << ", "
       << "LSN:" << lsn_ << ", "
       << "transID:" << txn_id_ << ", "
       << "prevLSN:" << prev_lsn_ << ", "
       << "LogType:" << static_cast<int>(log_record_type_) << "]";

    return os.str();
  }

 private:
  // the length of log record(for serialization, in bytes)
  int32_t size_{0};
  // must have fields
  lsn_t lsn_{INVALID_LSN};
  txn_id_t txn_id_{INVALID_TXN_ID};
  lsn_t prev_lsn_{INVALID_LSN};
  LogRecordType log_record_type_{LogRecordType::INVALID};

  // case1: for delete operation, delete_tuple_ for UNDO operation
  RID delete_rid_;
  Tuple delete_tuple_;

  // case2: for insert operation
  RID insert_rid_;
  Tuple insert_tuple_;

  // case3: for update operation
  RID update_rid_;
  Tuple old_tuple_;
  Tuple new_tuple_;

  // case4: for new page operation
  page_id_t prev_page_id_{INVALID_PAGE_ID};
  page_id_t page_id_{INVALID_PAGE_ID};
  static const int HEADER_SIZE = 20;
};  // namespace bustub

}  // namespace bustub
