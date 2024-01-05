//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include "common/exception.h"
#include "common/macros.h"
#include "storage/page/page_guard.h"

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, size_t replacer_k,
                                     LogManager *log_manager)
    : pool_size_(pool_size), disk_scheduler_(std::make_unique<DiskScheduler>(disk_manager)), log_manager_(log_manager) {
  // TODO(students): remove this line after you have implemented the buffer pool manager

  // we allocate a consecutive memory space for the buffer pool
  pages_ = new Page[pool_size_];
  replacer_ = std::make_unique<LRUKReplacer>(pool_size, replacer_k);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() { delete[] pages_; }

auto BufferPoolManager::NewPage(page_id_t *page_id) -> Page * {
  std::lock_guard<std::mutex> lock(latch_);  // Protect shared data with a lock.

  // Check if there are any free frames or any frames that can be evicted.
  if (free_list_.empty() && replacer_->Size() == 0) {
    *page_id = INVALID_PAGE_ID;  // No available frames.
//    throw Exception("Fuck you");
//    std::cout<<"hhh"<<std::endl;
    return nullptr;  // No new page can be created.
  }

  frame_id_t frame_id = -1;
  // Find a free frame or evict one.
  if (!free_list_.empty()) {
    frame_id = free_list_.front();
    free_list_.pop_front();
  } else {
    bool success = replacer_->Evict(&frame_id);  // Attempt to evict a frame.

    if (!success || frame_id == -1) {
      *page_id = INVALID_PAGE_ID;
      return nullptr;  // Failed to evict a frame.
    }

  }

  // Flush the page to disk if it is dirty before reusing the frame.
  Page *frame = &pages_[frame_id];
  if (frame->IsDirty()) {
    FlushPage(frame->GetPageId());
  }


//  page_table_.erase(frame_id);
  for(auto it:page_table_){
    if(it.second == frame_id){
      page_table_.erase(it.first);
      break;
    }
  }


  // Allocate a new page ID and set up the page.
  *page_id = AllocatePage();
  page_table_[*page_id] = frame_id;  // Link new page ID with the frame in the page table.

  // Reset the page contents and metadata.
  frame->ResetMemory();  // Reset the page memory to default values.
  frame->page_id_ = *page_id;  // Set the page ID of the page.
  frame->is_dirty_ = false;  // New page is not dirty.
  frame->pin_count_ = 1;  // New page starts with a pin count of 1.

  // Indicate to the replacer that this frame is not evictable until it is unpinned.
  replacer_->SetEvictable(frame_id, false);
  replacer_->RecordAccess(frame_id);
  // Return a pointer to the new page.
  return frame;
}

auto BufferPoolManager::FetchPage(page_id_t page_id, [[maybe_unused]] AccessType access_type) -> Page * {
  std::lock_guard<std::mutex> lock(latch_);

  // Check if page is already in buffer pool.
  auto page_table_it = page_table_.find(page_id);
  if(page_table_it != page_table_.end()) {
        frame_id_t frame_id = page_table_it->second;
        replacer_->SetEvictable(frame_id, false);
        pages_[frame_id].pin_count_++;
        return &pages_[frame_id];
  }

  // Find a frame for replacement.
  frame_id_t frame_id = INVALID_PAGE_ID;
  if(!free_list_.empty()) {
        frame_id = free_list_.front();
        free_list_.pop_front();
  } else if(replacer_->Evict(&frame_id)) {
        // Evict an old page.
        if(pages_[frame_id].IsDirty()) {
          // Flush dirty page to disk.
          FlushPage(pages_[frame_id].GetPageId());
        }
        // Remove from page table.
        page_table_.erase(pages_[frame_id].GetPageId());
  }

  if(frame_id != INVALID_PAGE_ID) {
        // Load the requested page into the frame.
        Page *page = &pages_[frame_id];
//        disk_scheduler_->ScheduleRead(page, page_id);
        auto promise = disk_scheduler_->CreatePromise();
        auto future = promise.get_future();
        DiskRequest request = {false, page->data_, page_id,std::move(promise)};
        disk_scheduler_->Schedule(std::move(request));

        if(future.get()){
          page->page_id_ = page_id;
          page->pin_count_ = 1;  // Pin the page.
          page->is_dirty_ = false;
          page_table_[page_id] = frame_id;
          replacer_->SetEvictable(frame_id, false);
          replacer_->RecordAccess(frame_id);
          return page;
        }
  }

  // No available frame or eviction failed.
  return nullptr;
}



auto BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty, [[maybe_unused]] AccessType access_type) -> bool {
  std::lock_guard<std::mutex> lock(latch_);

  auto page_table_it = page_table_.find(page_id);
  if(page_table_it == page_table_.end()) {
        return false;  // Page not found.
  }

  frame_id_t frame_id = page_table_it->second;
  Page *page = &pages_[frame_id];
  if(page->pin_count_ <= 0) {
        return false;  // Page not pinned.
  }

  page->pin_count_--;
  if(page->pin_count_ == 0) {
        replacer_->SetEvictable(frame_id, true);
  }
  if(is_dirty) {
        page->is_dirty_ = true;
  }

  return true;
}


auto BufferPoolManager::FlushPage(page_id_t page_id) -> bool {
  auto page_table_it = page_table_.find(page_id);
  if(page_table_it == page_table_.end()) {
        return false;  // Page not found.
  }

  frame_id_t frame_id = page_table_it->second;
  Page *page = &pages_[frame_id];
  if(page->is_dirty_) {
        auto promise = disk_scheduler_->CreatePromise();
        auto future = promise.get_future();
        DiskRequest request = {true, page->data_, page_id, std::move(promise)};
        disk_scheduler_->Schedule(std::move(request));

        if (future.get()) {
          // The write operation was successful.
            page->is_dirty_ = false;
        }else{
            throw Exception("Write failure");
        }
//        auto promise_read = disk_scheduler_->CreatePromise();
//        auto future_read = promise_read.get_future();
//        char data[BUSTUB_PAGE_SIZE] = {0};
//        DiskRequest request_read = {false, data, page_id,std::move(promise_read)};
//        disk_scheduler_->Schedule(std::move(request_read));
//        if(!future_read.get()){
//            throw Exception("Read failure!");
//        }
//        std::cout<<"page_data: "<<page->data_<<std::endl<<"data: "<<data<<std::endl<<sizeof(page->data_)<<std::endl;
  }
  return true;
}

void BufferPoolManager::FlushAllPages() {

  for (auto &page_entry : page_table_) {
        page_id_t page_id = page_entry.first;
        frame_id_t frame_id = page_entry.second;
        Page *page = &pages_[frame_id];

        if (page->is_dirty_) {

            auto promise = disk_scheduler_->CreatePromise();
            auto future = promise.get_future();
            DiskRequest request = {true, page->data_, page_id,std::move(promise)};
            disk_scheduler_->Schedule(std::move(request));
            if(future.get()) {
              page->is_dirty_ = false;  // Clear the dirty flag after flushing
            }
        }
  }
}

auto BufferPoolManager::DeletePage(page_id_t page_id) -> bool {
  std::lock_guard<std::mutex> lock(latch_);
  auto page_table_it = page_table_.find(page_id);
  if(page_table_it == page_table_.end()) {
        return true;  // Page not found.
  }

  frame_id_t frame_id = page_table_it->second;
  Page *page = &pages_[frame_id];
  if(page->pin_count_ > 0) {
        return false;  // Page is pinned.
  }

  if(page->is_dirty_) {
        FlushPage(page_id);  // Flush before deletion.
  }
  DeallocatePage(page_id);
  page->ResetMemory();
  free_list_.push_back(frame_id);
  replacer_->SetEvictable(frame_id, true);
  page_table_.erase(page_id);
  return true;
}


auto BufferPoolManager::AllocatePage() -> page_id_t { return next_page_id_++; }

auto BufferPoolManager::FetchPageBasic(page_id_t page_id) -> BasicPageGuard { return {this, nullptr}; }

auto BufferPoolManager::FetchPageRead(page_id_t page_id) -> ReadPageGuard { return {this, nullptr}; }

auto BufferPoolManager::FetchPageWrite(page_id_t page_id) -> WritePageGuard { return {this, nullptr}; }

auto BufferPoolManager::NewPageGuarded(page_id_t *page_id) -> BasicPageGuard { return {this, nullptr}; }

}  // namespace bustub
