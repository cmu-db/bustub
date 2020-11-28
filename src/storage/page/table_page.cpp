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

#include "storage/page/table_page.h"

#include <cassert>

namespace bustub {

void TablePage::Init(page_id_t page_id, uint32_t page_size, page_id_t prev_page_id, LogManager *log_manager,
                     Transaction *txn) {
  // Set the page ID.
  memcpy(GetData(), &page_id, sizeof(page_id));
  // Log that we are creating a new page.
  if (enable_logging) {
    LogRecord log_record =
        LogRecord(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::NEWPAGE, prev_page_id, page_id);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }
  // Set the previous and next page IDs.
  SetPrevPageId(prev_page_id);
  SetNextPageId(INVALID_PAGE_ID);
  SetFreeSpacePointer(page_size);
  SetTupleCount(0);
}

bool TablePage::InsertTuple(const Tuple &tuple, RID *rid, Transaction *txn, LockManager *lock_manager,
                            LogManager *log_manager) {
  BUSTUB_ASSERT(tuple.size_ > 0, "Cannot have empty tuples.");
  // If there is not enough space, then return false.
  if (GetFreeSpaceRemaining() < tuple.size_ + SIZE_TUPLE) {
    return false;
  }

  // Try to find a free slot to reuse.
  uint32_t i;
  for (i = 0; i < GetTupleCount(); i++) {
    // If the slot is empty, i.e. its tuple has size 0,
    if (GetTupleSize(i) == 0) {
      // Then we break out of the loop at index i.
      break;
    }
  }

  // If there was no free slot left, and we cannot claim it from the free space, then we give up.
  if (i == GetTupleCount() && GetFreeSpaceRemaining() < tuple.size_ + SIZE_TUPLE) {
    return false;
  }

  // Otherwise we claim available free space..
  SetFreeSpacePointer(GetFreeSpacePointer() - tuple.size_);
  memcpy(GetData() + GetFreeSpacePointer(), tuple.data_, tuple.size_);

  // Set the tuple.
  SetTupleOffsetAtSlot(i, GetFreeSpacePointer());
  SetTupleSize(i, tuple.size_);

  rid->Set(GetTablePageId(), i);
  if (i == GetTupleCount()) {
    SetTupleCount(GetTupleCount() + 1);
  }

  // Write the log record.
  if (enable_logging) {
    BUSTUB_ASSERT(!txn->IsSharedLocked(*rid) && !txn->IsExclusiveLocked(*rid), "A new tuple should not be locked.");
    // Acquire an exclusive lock on the new tuple.
    bool locked = lock_manager->LockExclusive(txn, *rid);
    BUSTUB_ASSERT(locked, "Locking a new tuple should always work.");
    LogRecord log_record(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::INSERT, *rid, tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }
  return true;
}

bool TablePage::MarkDelete(const RID &rid, Transaction *txn, LockManager *lock_manager, LogManager *log_manager) {
  uint32_t slot_num = rid.GetSlotNum();
  // If the slot number is invalid, abort the transaction.
  if (slot_num >= GetTupleCount()) {
    if (enable_logging) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }

  uint32_t tuple_size = GetTupleSize(slot_num);
  // If the tuple is already deleted, abort the transaction.
  if (IsDeleted(tuple_size)) {
    if (enable_logging) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }

  if (enable_logging) {
    // Acquire an exclusive lock, upgrading from a shared lock if necessary.
    if (txn->IsSharedLocked(rid)) {
      if (!lock_manager->LockUpgrade(txn, rid)) {
        return false;
      }
    } else if (!txn->IsExclusiveLocked(rid) && !lock_manager->LockExclusive(txn, rid)) {
      return false;
    }
    Tuple dummy_tuple;
    LogRecord log_record(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::MARKDELETE, rid, dummy_tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }

  // Mark the tuple as deleted.
  if (tuple_size > 0) {
    SetTupleSize(slot_num, SetDeletedFlag(tuple_size));
  }
  return true;
}

bool TablePage::UpdateTuple(const Tuple &new_tuple, Tuple *old_tuple, const RID &rid, Transaction *txn,
                            LockManager *lock_manager, LogManager *log_manager) {
  BUSTUB_ASSERT(new_tuple.size_ > 0, "Cannot have empty tuples.");
  uint32_t slot_num = rid.GetSlotNum();
  // If the slot number is invalid, abort the transaction.
  if (slot_num >= GetTupleCount()) {
    if (enable_logging) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }
  uint32_t tuple_size = GetTupleSize(slot_num);
  // If the tuple is deleted, abort the transaction.
  if (IsDeleted(tuple_size)) {
    if (enable_logging) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }
  // If there is not enuogh space to update, we need to update via delete followed by an insert (not enough space).
  if (GetFreeSpaceRemaining() + tuple_size < new_tuple.size_) {
    return false;
  }

  // Copy out the old value.
  uint32_t tuple_offset = GetTupleOffsetAtSlot(slot_num);
  old_tuple->size_ = tuple_size;
  if (old_tuple->allocated_) {
    delete[] old_tuple->data_;
  }
  old_tuple->data_ = new char[old_tuple->size_];
  memcpy(old_tuple->data_, GetData() + tuple_offset, old_tuple->size_);
  old_tuple->rid_ = rid;
  old_tuple->allocated_ = true;

  if (enable_logging) {
    // Acquire an exclusive lock, upgrading from shared if necessary.
    if (txn->IsSharedLocked(rid)) {
      if (!lock_manager->LockUpgrade(txn, rid)) {
        return false;
      }
    } else if (!txn->IsExclusiveLocked(rid) && !lock_manager->LockExclusive(txn, rid)) {
      return false;
    }
    LogRecord log_record(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::UPDATE, rid, *old_tuple, new_tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }

  // Perform the update.
  uint32_t free_space_pointer = GetFreeSpacePointer();
  BUSTUB_ASSERT(tuple_offset >= free_space_pointer, "Offset should appear after current free space position.");

  memmove(GetData() + free_space_pointer + tuple_size - new_tuple.size_, GetData() + free_space_pointer,
          tuple_offset - free_space_pointer);
  SetFreeSpacePointer(free_space_pointer + tuple_size - new_tuple.size_);
  memcpy(GetData() + tuple_offset + tuple_size - new_tuple.size_, new_tuple.data_, new_tuple.size_);
  SetTupleSize(slot_num, new_tuple.size_);

  // Update all tuple offsets.
  for (uint32_t i = 0; i < GetTupleCount(); ++i) {
    uint32_t tuple_offset_i = GetTupleOffsetAtSlot(i);
    if (GetTupleSize(i) > 0 && tuple_offset_i < tuple_offset + tuple_size) {
      SetTupleOffsetAtSlot(i, tuple_offset_i + tuple_size - new_tuple.size_);
    }
  }
  return true;
}

void TablePage::ApplyDelete(const RID &rid, Transaction *txn, LogManager *log_manager) {
  uint32_t slot_num = rid.GetSlotNum();
  BUSTUB_ASSERT(slot_num < GetTupleCount(), "Cannot have more slots than tuples.");

  uint32_t tuple_offset = GetTupleOffsetAtSlot(slot_num);
  uint32_t tuple_size = GetTupleSize(slot_num);
  // Check if this is a delete operation, i.e. commit a delete.
  if (IsDeleted(tuple_size)) {
    tuple_size = UnsetDeletedFlag(tuple_size);
  }
  // Otherwise we are rolling back an insert.

  // We need to copy out the deleted tuple for undo purposes.
  Tuple delete_tuple;
  delete_tuple.size_ = tuple_size;
  delete_tuple.data_ = new char[delete_tuple.size_];
  memcpy(delete_tuple.data_, GetData() + tuple_offset, delete_tuple.size_);
  delete_tuple.rid_ = rid;
  delete_tuple.allocated_ = true;

  if (enable_logging) {
    BUSTUB_ASSERT(txn->IsExclusiveLocked(rid), "We must own the exclusive lock!");

    LogRecord log_record(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::APPLYDELETE, rid, delete_tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }

  uint32_t free_space_pointer = GetFreeSpacePointer();
  BUSTUB_ASSERT(tuple_offset >= free_space_pointer, "Free space appears before tuples.");

  memmove(GetData() + free_space_pointer + tuple_size, GetData() + free_space_pointer,
          tuple_offset - free_space_pointer);
  SetFreeSpacePointer(free_space_pointer + tuple_size);
  SetTupleSize(slot_num, 0);
  SetTupleOffsetAtSlot(slot_num, 0);

  // Update all tuple offsets.
  for (uint32_t i = 0; i < GetTupleCount(); ++i) {
    uint32_t tuple_offset_i = GetTupleOffsetAtSlot(i);
    if (GetTupleSize(i) != 0 && tuple_offset_i < tuple_offset) {
      SetTupleOffsetAtSlot(i, tuple_offset_i + tuple_size);
    }
  }
}

void TablePage::RollbackDelete(const RID &rid, Transaction *txn, LogManager *log_manager) {
  // Log the rollback.
  if (enable_logging) {
    BUSTUB_ASSERT(txn->IsExclusiveLocked(rid), "We must own an exclusive lock on the RID.");
    Tuple dummy_tuple;
    LogRecord log_record(txn->GetTransactionId(), txn->GetPrevLSN(), LogRecordType::ROLLBACKDELETE, rid, dummy_tuple);
    lsn_t lsn = log_manager->AppendLogRecord(&log_record);
    SetLSN(lsn);
    txn->SetPrevLSN(lsn);
  }

  uint32_t slot_num = rid.GetSlotNum();
  BUSTUB_ASSERT(slot_num < GetTupleCount(), "We can't have more slots than tuples.");
  uint32_t tuple_size = GetTupleSize(slot_num);

  // Unset the deleted flag.
  if (IsDeleted(tuple_size)) {
    SetTupleSize(slot_num, UnsetDeletedFlag(tuple_size));
  }
}

bool TablePage::GetTuple(const RID &rid, Tuple *tuple, Transaction *txn, LockManager *lock_manager) {
  // Get the current slot number.
  uint32_t slot_num = rid.GetSlotNum();
  // If somehow we have more slots than tuples, abort the transaction.
  if (slot_num >= GetTupleCount()) {
    if (enable_logging) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }
  // Otherwise get the current tuple size too.
  uint32_t tuple_size = GetTupleSize(slot_num);
  // If the tuple is deleted, abort the transaction.
  if (IsDeleted(tuple_size)) {
    if (enable_logging) {
      txn->SetState(TransactionState::ABORTED);
    }
    return false;
  }

  // Otherwise we have a valid tuple, try to acquire at least a shared lock.
  if (enable_logging) {
    if (!txn->IsSharedLocked(rid) && !txn->IsExclusiveLocked(rid) && !lock_manager->LockShared(txn, rid)) {
      return false;
    }
  }

  // At this point, we have at least a shared lock on the RID. Copy the tuple data into our result.
  uint32_t tuple_offset = GetTupleOffsetAtSlot(slot_num);
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

bool TablePage::GetFirstTupleRid(RID *first_rid) {
  // Find and return the first valid tuple.
  for (uint32_t i = 0; i < GetTupleCount(); ++i) {
    if (!IsDeleted(GetTupleSize(i))) {
      first_rid->Set(GetTablePageId(), i);
      return true;
    }
  }
  first_rid->Set(INVALID_PAGE_ID, 0);
  return false;
}

bool TablePage::GetNextTupleRid(const RID &cur_rid, RID *next_rid) {
  BUSTUB_ASSERT(cur_rid.GetPageId() == GetTablePageId(), "Wrong table!");
  // Find and return the first valid tuple after our current slot number.
  for (auto i = cur_rid.GetSlotNum() + 1; i < GetTupleCount(); ++i) {
    if (!IsDeleted(GetTupleSize(i))) {
      next_rid->Set(GetTablePageId(), i);
      return true;
    }
  }
  // Otherwise return false as there are no more tuples.
  next_rid->Set(INVALID_PAGE_ID, 0);
  return false;
}
}  // namespace bustub
