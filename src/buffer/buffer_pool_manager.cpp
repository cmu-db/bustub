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
#include <unordered_map>

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // We allocate a consecutive memory space for the buffer pool.
  pages_ = new Page[pool_size_];
  replacer_ = new ClockReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  if (page_table_.count(page_id) != 0U) {
    frame_id_t frame_id = page_table_[page_id];
    replacer_->Pin(frame_id);
    Page *page = pages_ + frame_id;
    page->pin_count_++;
    return page;
  }
  // 查找一个空页或者淘汰一个
  frame_id_t empty_frame;
  Page *page;
  if (free_list_.empty()) {
    // 空队列已空，从缓存中获取
    if (!replacer_->Victim(&empty_frame)) {
      return nullptr;
    }
    // 获取page_id驱逐
    page = pages_ + empty_frame;
    page_id_t old_page_id = page->page_id_;
    // dirty将其写回disk
    if (page->IsDirty()) {
      FlushPage(old_page_id);
    }
    page_table_.erase(old_page_id);
  } else {
    // 去前面一个
    empty_frame = free_list_.front();
    free_list_.pop_front();
    page = pages_ + empty_frame;
  }
  page->ResetMemory();
  // 设置一下page_table_
  page_table_[page_id] = empty_frame;
  page->page_id_ = page_id;
  page->pin_count_ = 1;
  // 获取到了一个空页，读取空页
  disk_manager_->ReadPage(page_id, reinterpret_cast<char *>(page));

  return page;
}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  // is_dirty是做啥的
  frame_id_t frame_id = page_table_[page_id];
  Page *page = pages_ + frame_id;
  page->pin_count_--;
  // 脏数据进行||运算
  page->is_dirty_ = page->is_dirty_ || is_dirty;
  if (page->pin_count_ == 0) {
    // ok这页我不需要了
    replacer_->Unpin(frame_id);
  }
  return page->pin_count_ <= 0;
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
  // 判断是否存在
  if (page_table_.count(page_id) == 0U) {
    return false;
  }
  frame_id_t frame_id = page_table_[page_id];
  Page *page = pages_ + frame_id;
  disk_manager_->WritePage(page_id, reinterpret_cast<char *>(page));
  return true;
}

Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  // 0.   Make sure you call DiskManager::AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.
  // 找一个空的page
  Page *page;
  *page_id = INVALID_PAGE_ID;
  frame_id_t frame_id;
  //  page_id_t page_id;
  if (free_list_.empty()) {
    // 从replacer取一个
    if (!replacer_->Victim(&frame_id)) {
      return nullptr;
    }
    page = pages_ + frame_id;
    if (page->is_dirty_) {
      FlushPage(page->page_id_);
    }
    page_table_.erase(page->page_id_);
  } else {
    // 从freelist取一个
    frame_id = free_list_.front();
    free_list_.pop_front();
    page = pages_ + frame_id;
  }
  page->ResetMemory();
  page->page_id_ = *page_id = disk_manager_->AllocatePage();
  page->pin_count_ = 1;
  page_table_[page->page_id_] = frame_id;

  return page;
}

bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // 0.   Make sure you call DiskManager::DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
  // 向free_list添加一个page
  if (page_table_.count(page_id) == 0U) {
    return true;
  }
  Page *page = pages_ + page_table_[page_id];
  // 有人在用，返回false
  if (page->GetPinCount() != 0) {
    return false;
  }
  // 加入free list
  free_list_.emplace_back(page_table_[page_id]);
  page_table_.erase(page_id);
  // 清除磁盘数据和内存数据
  disk_manager_->DeallocatePage(page_id);
  page->ResetMemory();

  return true;
}

void BufferPoolManager::FlushAllPagesImpl() {
  // You can do it!
  // 将所有dirty页刷入磁盘
  for (auto &idx : page_table_) {
    // 获取page
    FlushPage(idx.first);
  }
}

}  // namespace bustub
