//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include <list>

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  replacer_ = new LRUReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
}

frame_id_t BufferPoolManager::GetAvailablePage() {
  frame_id_t frame_id;
  Page *page_ptr;

  if (!free_list_.empty()) {
    frame_id = free_list_.front();
    free_list_.pop_front();

    return frame_id;
  }

  if (replacer_->Size() > 0) {
    replacer_->Victim(&frame_id);
    page_ptr = GetPage(frame_id);
    page_id_t replace_page_id = page_ptr->GetPageId();

    if (page_ptr->IsDirty()) {
      disk_manager_->WritePage(replace_page_id, page_ptr->GetData());
    }
    page_table_.erase(replace_page_id);

    return frame_id;
  }

  return INVALID_FRAME_ID;
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // 0.     Dyy think I should make sure I call DiskManager::WritePage and DiskManager::ReadPage!
  // 0.1    Maybe I should also pin that page?
  // 0.2    Well, it seems I should modify `page.h` first...
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.

  latch_.lock();

  frame_id_t frame_id;
  Page *page_ptr;

  frame_id = GetFrame(page_id);
  if (frame_id != INVALID_FRAME_ID) {
    page_ptr = GetPage(frame_id);
    page_ptr->AddPinCount();
    replacer_->Pin(frame_id);

    latch_.unlock();
    return page_ptr;
  }

  frame_id = GetAvailablePage();
  if (frame_id == INVALID_FRAME_ID) {
    latch_.unlock();
    return nullptr;
  }

  page_table_.insert({page_id, frame_id});
  page_ptr = GetPage(frame_id);
  page_ptr->SetPageId(page_id);
  page_ptr->SetPinCount(1);
  page_ptr->SetDirty(false);
  disk_manager_->ReadPage(page_id, page_ptr->GetData());

  latch_.unlock();
  return page_ptr;
}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  latch_.lock();

  frame_id_t frame_id = GetFrame(page_id);
  assert(frame_id != INVALID_FRAME_ID);
  Page *page_ptr = GetPage(frame_id);

  if (is_dirty) {
    page_ptr->SetDirty(true);
  }

  int pin_count = page_ptr->GetPinCount();
  if (pin_count <= 0) {
    latch_.unlock();
    return false;
  }

  pin_count = page_ptr->SubPinCount();
  if (pin_count == 0) {
    replacer_->Unpin(frame_id);
  }

  latch_.unlock();
  return true;
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // 0. Make sure you call DiskManager::WritePage! (if dirty)
  // 1. set page id to INVALID_PAGE_ID, and dirty_flag and pin_count
  // 2. modify free_list and page_table and replacer
  assert(page_id != INVALID_PAGE_ID);

  latch_.lock();

  frame_id_t frame_id = GetFrame(page_id);
  if (frame_id == INVALID_FRAME_ID) {
    latch_.unlock();
    return false;
  }

  Page *page_ptr = GetPage(frame_id);

  if (page_ptr->IsDirty()) {
    disk_manager_->WritePage(page_id, page_ptr->GetData());
  }

  page_ptr->Reset();

  page_table_.erase(page_id);
  free_list_.push_back(frame_id);
  replacer_->Pin(frame_id);

  latch_.unlock();
  return true;
}

Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  // 0.   Make sure you call DiskManager::AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.
  latch_.lock();

  frame_id_t frame_id = GetAvailablePage();
  if (frame_id == INVALID_FRAME_ID) {
    latch_.unlock();
    return nullptr;
  }

  Page *page_ptr = GetPage(frame_id);
  page_id_t new_page_id = disk_manager_->AllocatePage();
  page_ptr->Reset();
  page_ptr->SetPageId(new_page_id);
  page_ptr->SetPinCount(1);

  page_table_.insert({new_page_id, frame_id});

  *page_id = new_page_id;

  latch_.unlock();
  return page_ptr;
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // 0.   Make sure you call DiskManager::DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.

  assert(page_id != INVALID_FRAME_ID);
  latch_.lock();

  frame_id_t frame_id = GetFrame(page_id);
  if (frame_id == INVALID_FRAME_ID) {
    disk_manager_->DeallocatePage(page_id);
    latch_.unlock();
    return true;
  }

  Page *page_ptr = GetPage(frame_id);
  if (page_ptr->GetPinCount() > 0) {
    latch_.unlock();
    return false;
  }

  disk_manager_->DeallocatePage(page_id);
  page_table_.erase(page_id);
  replacer_->Pin(frame_id);
  page_ptr->Reset();
  free_list_.push_back(frame_id);

  latch_.unlock();
  return true;
}

void BufferPoolManager::FlushAllPagesImpl() {
  // You can do it!

  for (const auto &page_pair : page_table_) {
    FlushPageImpl(page_pair.first);
  }
}

}  // namespace bustub
