//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_page.h
//
// Identification: src/include/storage/page/table_page.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstring>

#include "common/rid.h"
#include "concurrency/lock_manager.h"
#include "recovery/log_manager.h"
#include "storage/page/page.h"
#include "storage/table/tuple.h"

#define DELETE_MASK (1 << (8 * sizeof(uint32_t) - 1))

namespace bustub {

/**
 * Slotted page format:
 *  --------------------------------------------------------
 * | HEADER | ... FREE SPACE ... | ... INSERTED TUPLES ... |
 *  --------------------------------------------------------
 *                               ^
 *                               free space pointer
 *
 *  Header format (size in byte):
 *  --------------------------------------------------------------------------
 * | PageId (4)| LSN (4)| PrevPageId (4)| NextPageId (4)| FreeSpacePointer(4) |
 *  --------------------------------------------------------------------------
 *  --------------------------------------------------------------
 * | TupleCount (4) | Tuple_1 offset (4) | Tuple_1 size (4) | ... |
 *  --------------------------------------------------------------
 *
 */
class TablePage : public Page {
 public:
  /**
   * Initialize the header
   */
  void Init(page_id_t page_id, size_t page_size, page_id_t prev_page_id, LogManager *log_manager, Transaction *txn);

  /**
   * Return the page id
   */
  page_id_t GetPageId();

  /**
   * Return the page id of the previous page in the table.
   */
  page_id_t GetPrevPageId();

  /**
   * Return the page id of the next page in the table
   */
  page_id_t GetNextPageId();

  /**
   * Sets the page id of the previous page in the table
   */
  void SetPrevPageId(page_id_t prev_page_id);

  /**
   * Sets the page id of the next page in the table.
   */
  void SetNextPageId(page_id_t next_page_id);

  /**
   * Insert a tuple into the table
   * @param tuple tuple to insert
   * @param rid[out] rid of the inserted tuple
   * @param txn transaction performing the insert
   * @param lock_manager the lock manager
   * @param log_manager the log manager
   * @return whether the insert is successful (i.e. there is enough space)
   */
  bool InsertTuple(const Tuple &tuple, RID *rid, Transaction *txn, LockManager *lock_manager, LogManager *log_manager);

  /**
   * Mark a tuple as deleted.
   * @param rid rid of the tuple to delete
   * @param txn transaction performing the delete
   * @param lock_manager the lock manager
   * @param log_manager the log manager
   * @return whether the delete is successful or not (i.e the tuple exists)
   */
  bool MarkDelete(const RID &rid, Transaction *txn, LockManager *lock_manager, LogManager *log_manager);

  /**
   * Update a tuple
   * @param new_tuple new value of the tuple
   * @param old_tuple[out] old value of the tuple
   * @param rid rid of the tuple
   * @param txn transaction performing the update
   * @param lock_manager the lock manager
   * @param log_manager the log manager
   * @return
   */
  bool UpdateTuple(const Tuple &new_tuple, Tuple *old_tuple, const RID &rid, Transaction *txn,
                   LockManager *lock_manager, LogManager *log_manager);

  /**
   * Called at commit/abort actually perform delete or rollback an insert.
   */
  void ApplyDelete(const RID &rid, Transaction *txn, LogManager *log_manager);

  /**
   * Called on abort to rollback a delete.
   */
  void RollbackDelete(const RID &rid, Transaction *txn, LogManager *log_manager);

  /**
   * Read a tuple from a table
   * @param rid rid of the tuple to read
   * @param tuple[out] the tuple that is read
   * @param txn transaction performing the read
   * @param lock_manager the lock manager
   * @return true if the read is successful (i.e. the tuple exists)
   */
  bool GetTuple(const RID &rid, Tuple *tuple, Transaction *txn, LockManager *lock_manager);

  /**
   * Return the rid of first tuple in this page.
   */
  bool GetFirstTupleRid(RID *first_rid);

  /**
   * Return the next rid given the current one.
   */
  bool GetNextTupleRid(const RID &cur_rid, RID *next_rid);

 private:
  /**
   * helper functions
   */
  uint32_t GetTupleOffset(uint32_t slot_num);
  uint32_t GetTupleSize(uint32_t slot_num);
  void SetTupleOffset(uint32_t slot_num, uint32_t offset);
  void SetTupleSize(uint32_t slot_num, uint32_t offset);
  uint32_t GetFreeSpacePointer();  // offset of the beginning of free space
  void SetFreeSpacePointer(uint32_t free_space_pointer);
  uint32_t GetTupleCount();  // Note that this tuple count may be larger than # of
                             // actual tuples because some slots may be empty
  void SetTupleCount(uint32_t tuple_count);
  uint32_t GetFreeSpaceSize();

  // Utility
  bool IsDeleted(uint32_t tuple_size) { return static_cast<bool>(tuple_size & DELETE_MASK) || tuple_size == 0; }

  uint32_t SetDeletedFlag(uint32_t tuple_size) { return static_cast<uint32_t>(tuple_size | DELETE_MASK); }

  uint32_t UnsetDeletedFlag(uint32_t tuple_size) { return static_cast<uint32_t>(tuple_size & (~DELETE_MASK)); }
};
}  // namespace bustub
