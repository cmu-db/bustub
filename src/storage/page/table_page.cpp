//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// table_page.cpp
//
// Identification: src/storage/page/table_page.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cassert>

#include "storage/page/table_page.h"

namespace bustub {
/**
 * Header related
 */
void TablePage::Init(page_id_t page_id, size_t page_size, page_id_t prev_page_id, LogManager *log_manager,
                     Transaction *txn) {
  memcpy(GetData(), &page_id, 4);  // set page_id
  if (ENABLE_LOGGING) {
    LogRecord log_record = LogRecord(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::NEWPAGE, prev_page_id);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }
  SetPrevPageId(prev_page_id);
  SetNextPageId(INVALID_PAGE_ID);
  SetFreeSpacePointer(page_size);
  SetTupleCount(0);
}

page_id_t TablePage::GetPageId() { return *reinterpret_cast<page_id_t *>(GetData()); }

page_id_t TablePage::GetPrevPageId() { return *reinterpret_cast<page_id_t *>(GetData() + 8); }

page_id_t TablePage::GetNextPageId() { return *reinterpret_cast<page_id_t *>(GetData() + 12); }

void TablePage::SetPrevPageId(page_id_t prev_page_id) { memcpy(GetData() + 8, &prev_page_id, 4); }

void TablePage::SetNextPageId(page_id_t next_page_id) { memcpy(GetData() + 12, &next_page_id, 4); }

/**
 * Tuple related
 */
bool TablePage::InsertTuple(const Tuple &tuple, RID *rid, Transaction *txn, LockManager *lock_manager,
                            LogManager *log_manager) {
  BUSTUB_ASSERT(tuple.size_ > 0, "Cannot have empty tuples");
  if (GetFreeSpaceSize() < tuple.size_) {
    return false;  // not enough space
  }

  // try to reuse a free slot first
  uint32_t i;
  for (i = 0; i < GetTupleCount(); ++i) {
    if (GetTupleSize(i) == 0) {  // empty slot
      rid->Set(GetPageId(), i);
      if (ENABLE_LOGGING) {
        assert(txn->GetSharedLockSet()->find(*rid) == txn->GetSharedLockSet()->end() &&
               txn->GetExclusiveLockSet()->find(*rid) == txn->GetExclusiveLockSet()->end());
      }
      break;
    }
  }

  // no free slot left
  if (i == GetTupleCount() && GetFreeSpaceSize() < tuple.size_ + 8) {
    return false;  // not enough space
  }

  SetFreeSpacePointer(GetFreeSpacePointer() - tuple.size_);  // update free space pointer first
  memcpy(GetData() + GetFreeSpacePointer(), tuple.data_, tuple.size_);
  SetTupleOffset(i, GetFreeSpacePointer());
  SetTupleSize(i, tuple.size_);
  if (i == GetTupleCount()) {
    rid->Set(GetPageId(), i);
    SetTupleCount(GetTupleCount() + 1);
  }
  // write the log after set rid
  if (ENABLE_LOGGING) {
    // acquire the exclusive lock
    bool locked = lock_manager->LockExclusive(txn, *rid);
    BUSTUB_ASSERT(locked, "Locking a new tuple should always work");
    LogRecord log_record = LogRecord(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::INSERT, *rid, tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }
  return true;
}

/*
 * MarkDelete method does not truly delete a tuple from table page
 * Instead it set the tuple as 'deleted' by changing the tuple size metadata to
 * negative so that no other transaction can reuse this slot
 *
 */
bool TablePage::MarkDelete(const RID &rid, Transaction *txn, LockManager *lock_manager, LogManager *log_manager) {
  uint32_t slot_num = rid.GetSlotNum();
  if (slot_num >= GetTupleCount()) {
    if (ENABLE_LOGGING) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }

  uint32_t tuple_size = GetTupleSize(slot_num);
  if (IsDeleted(tuple_size)) {
    if (ENABLE_LOGGING) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }

  if (ENABLE_LOGGING) {
    // acquire exclusive lock
    // if has shared lock
    if (txn->GetSharedLockSet()->find(rid) != txn->GetSharedLockSet()->end()) {
      if (!lock_manager->LockUpgrade(txn, rid)) {
        return false;
      }
    } else if (txn->GetExclusiveLockSet()->find(rid) == txn->GetExclusiveLockSet()->end() &&
               !lock_manager->LockExclusive(txn, rid)) {  // no shared lock
      return false;
    }
    Tuple dummy_tuple;
    LogRecord log_record =
        LogRecord(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::MARKDELETE, rid, dummy_tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }

  // set tuple size to negative value
  auto marked_size = SetDeletedFlag(tuple_size);
  if (tuple_size > 0) {
    SetTupleSize(slot_num, marked_size);
  }
  return true;
}

bool TablePage::UpdateTuple(const Tuple &new_tuple, Tuple *old_tuple, const RID &rid, Transaction *txn,
                            LockManager *lock_manager, LogManager *log_manager) {
  BUSTUB_ASSERT(new_tuple.size_ > 0, "Cannot have empty tuples");
  uint32_t slot_num = rid.GetSlotNum();
  if (slot_num >= GetTupleCount()) {
    if (ENABLE_LOGGING) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }
  uint32_t tuple_size = GetTupleSize(slot_num);  // old tuple size
  if (IsDeleted(tuple_size)) {
    if (ENABLE_LOGGING) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }
  if (GetFreeSpaceSize() < new_tuple.size_ - tuple_size) {
    // should delete/insert because not enough space
    return false;
  }

  // copy out old value
  uint32_t tuple_offset = GetTupleOffset(slot_num);  // the tuple offset of the old tuple
  old_tuple->size_ = tuple_size;
  if (old_tuple->allocated_) {
    delete[] old_tuple->data_;
  }
  old_tuple->data_ = new char[old_tuple->size_];
  memcpy(old_tuple->data_, GetData() + tuple_offset, old_tuple->size_);
  old_tuple->rid_ = rid;
  old_tuple->allocated_ = true;

  if (ENABLE_LOGGING) {
    // acquire exclusive lock
    // if has shared lock
    if (txn->GetSharedLockSet()->find(rid) != txn->GetSharedLockSet()->end()) {
      if (!lock_manager->LockUpgrade(txn, rid)) {
        return false;
      }
    } else if (txn->GetExclusiveLockSet()->find(rid) == txn->GetExclusiveLockSet()->end() &&
               !lock_manager->LockExclusive(txn, rid)) {  // no shared lock
      return false;
    }
    LogRecord log_record =
        LogRecord(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::UPDATE, rid, *old_tuple, new_tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }

  // update
  uint32_t free_space_pointer = GetFreeSpacePointer();  // old pointer to the free space
  assert(tuple_offset >= free_space_pointer);
  memmove(GetData() + free_space_pointer + tuple_size - new_tuple.size_, GetData() + free_space_pointer,
          tuple_offset - free_space_pointer);
  SetFreeSpacePointer(free_space_pointer + tuple_size - new_tuple.size_);
  memcpy(GetData() + tuple_offset + tuple_size - new_tuple.size_, new_tuple.data_,
         new_tuple.size_);                          // copy new tuple
  SetTupleSize(slot_num, new_tuple.size_);          // update tuple size in slot
  for (uint32_t i = 0; i < GetTupleCount(); ++i) {  // update tuple offsets (including the updated one)
    uint32_t tuple_offset_i = GetTupleOffset(i);
    if (GetTupleSize(i) > 0 && tuple_offset_i < tuple_offset + tuple_size) {
      SetTupleOffset(i, tuple_offset_i + tuple_size - new_tuple.size_);
    }
  }
  return true;
}

/*
 * ApplyDelete function truly delete a tuple from table page, and make the slot
 * available for use again.
 * This function is called when a transaction commits or when you undo insert
 */
void TablePage::ApplyDelete(const RID &rid, Transaction *txn, LogManager *log_manager) {
  uint32_t slot_num = rid.GetSlotNum();
  assert(slot_num < GetTupleCount());
  // the tuple offset of the deleted tuple
  uint32_t tuple_offset = GetTupleOffset(slot_num);
  uint32_t tuple_size = GetTupleSize(slot_num);
  if (IsDeleted(tuple_size)) {  // commit delete
    tuple_size = UnsetDeletedFlag(tuple_size);
  }  // else: rollback insert op

  // copy out delete value, for undo purpose
  Tuple delete_tuple;
  delete_tuple.size_ = tuple_size;
  delete_tuple.data_ = new char[delete_tuple.size_];
  memcpy(delete_tuple.data_, GetData() + tuple_offset, delete_tuple.size_);
  delete_tuple.rid_ = rid;
  delete_tuple.allocated_ = true;

  if (ENABLE_LOGGING) {
    // must already grab the exclusive lock
    assert(txn->GetExclusiveLockSet()->find(rid) != txn->GetExclusiveLockSet()->end());

    LogRecord log_record =
        LogRecord(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::APPLYDELETE, rid, delete_tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }

  uint32_t free_space_pointer = GetFreeSpacePointer();  // old pointer to the free space
  assert(tuple_offset >= free_space_pointer);
  memmove(GetData() + free_space_pointer + tuple_size, GetData() + free_space_pointer,
          tuple_offset - free_space_pointer);
  SetFreeSpacePointer(free_space_pointer + tuple_size);
  SetTupleSize(slot_num, 0);
  SetTupleOffset(slot_num, 0);  // invalid offset
  for (uint32_t i = 0; i < GetTupleCount(); ++i) {
    uint32_t tuple_offset_i = GetTupleOffset(i);
    if (GetTupleSize(i) != 0 && tuple_offset_i < tuple_offset) {
      SetTupleOffset(i, tuple_offset_i + tuple_size);
    }
  }
}

/*
 * RollbackDelete is a complementary function wrt MarkDelete function.
 * It flip the tuple size from negative to positive, so that the tuple becomes
 * visible again.
 * This function is called when abort a transaction
 */
void TablePage::RollbackDelete(const RID &rid, Transaction *txn, LogManager *log_manager) {
  if (ENABLE_LOGGING) {
    // must have already grab the exclusive lock
    assert(txn->GetExclusiveLockSet()->find(rid) != txn->GetExclusiveLockSet()->end());

    Tuple dummy_tuple;
    LogRecord log_record =
        LogRecord(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::ROLLBACKDELETE, rid, dummy_tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }

  uint32_t slot_num = rid.GetSlotNum();
  assert(slot_num < GetTupleCount());
  uint32_t tuple_size = GetTupleSize(slot_num);

  // set tuple size to positive value
  if (IsDeleted(tuple_size)) {
    SetTupleSize(slot_num, UnsetDeletedFlag(tuple_size));
  }
}

bool TablePage::GetTuple(const RID &rid, Tuple *tuple, Transaction *txn, LockManager *lock_manager) {
  uint32_t slot_num = rid.GetSlotNum();
  if (slot_num >= GetTupleCount()) {
    if (ENABLE_LOGGING) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }
  uint32_t tuple_size = GetTupleSize(slot_num);
  if (IsDeleted(tuple_size)) {
    if (ENABLE_LOGGING) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }

  if (ENABLE_LOGGING) {
    // acquire shared lock
    if (txn->GetExclusiveLockSet()->find(rid) == txn->GetExclusiveLockSet()->end() &&
        txn->GetSharedLockSet()->find(rid) == txn->GetSharedLockSet()->end() && !lock_manager->LockShared(txn, rid)) {
      return false;
    }
  }

  uint32_t tuple_offset = GetTupleOffset(slot_num);
  tuple->size_ = tuple_size;
  if (tuple->allocated_) {
    delete[] tuple->data_;
  }
  tuple->data_ = new char[tuple->size_];
  memcpy(tuple->data_, GetData() + tuple_offset, tuple->size_);
  tuple->rid_ = rid;
  tuple->allocated_ = true;
  return true;
}

/**
 * Tuple iterator
 */
bool TablePage::GetFirstTupleRid(RID *first_rid) {
  for (uint32_t i = 0; i < GetTupleCount(); ++i) {
    if (GetTupleSize(i) > 0) {  // valid tuple
      first_rid->Set(GetPageId(), i);
      return true;
    }
  }
  // there is no tuple within current page
  first_rid->Set(INVALID_PAGE_ID, 0);
  return false;
}

bool TablePage::GetNextTupleRid(const RID &cur_rid, RID *next_rid) {
  assert(cur_rid.GetPageId() == GetPageId());
  for (auto i = cur_rid.GetSlotNum() + 1; i < GetTupleCount(); ++i) {
    if (GetTupleSize(i) > 0) {  // valid tuple
      next_rid->Set(GetPageId(), i);
      return true;
    }
  }
  return false;  // End of last tuple
}

/**
 * helper functions
 */

// tuple slots
uint32_t TablePage::GetTupleOffset(uint32_t slot_num) {
  return *reinterpret_cast<uint32_t *>(GetData() + 24 + 8 * slot_num);
}

uint32_t TablePage::GetTupleSize(uint32_t slot_num) {
  return *reinterpret_cast<uint32_t *>(GetData() + 28 + 8 * slot_num);
}

void TablePage::SetTupleOffset(uint32_t slot_num, uint32_t offset) {
  memcpy(GetData() + 24 + 8 * slot_num, &offset, 4);
}

void TablePage::SetTupleSize(uint32_t slot_num, uint32_t offset) { memcpy(GetData() + 28 + 8 * slot_num, &offset, 4); }

// free space
uint32_t TablePage::GetFreeSpacePointer() { return *reinterpret_cast<uint32_t *>(GetData() + 16); }

void TablePage::SetFreeSpacePointer(uint32_t free_space_pointer) { memcpy(GetData() + 16, &free_space_pointer, 4); }

// tuple count
uint32_t TablePage::GetTupleCount() { return *reinterpret_cast<uint32_t *>(GetData() + 20); }

void TablePage::SetTupleCount(uint32_t tuple_count) { memcpy(GetData() + 20, &tuple_count, 4); }

// for free space calculation
uint32_t TablePage::GetFreeSpaceSize() { return GetFreeSpacePointer() - 24 - GetTupleCount() * 8; }
}  // namespace bustub
