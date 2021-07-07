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
//  replacer_ = new LRUReplacer(pool_size);
  replacer_ = new ClockReplacer(pool_size);

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() {
  delete[] pages_;
  delete replacer_;
  // TODO: nullify all Page Objects
}

Page *BufferPoolManager::FetchPageImpl(page_id_t page_id) {
  // 1.     Search the page table for the requested page (P).
  // 1.1    If P exists, pin it and return it immediately.
  // 1.2    If P does not exist, find a replacement page (R) from either the free list or the replacer.
  //        Note that pages are always found from the free list first.
  // 2.     If R is dirty, write it back to the disk.
  // 3.     Delete R from the page table and insert P.
  // 4.     Update P's metadata, read in the page content from disk, and then return a pointer to P.
  latch_.lock();
  // search for requested p in page table
  for (auto i = 0; i < (int)pool_size_; i++) {
    if(pages_[i].page_id_!=INVALID_PAGE_ID && pages_[i].page_id_ == page_id) {
      pages_[i].pin_count_++;
      replacer_->Pin(i);
      latch_.unlock();
      std::cout<< "Our stupid fetch page result data is: " <<pages_[page_id].GetData()<<std::endl;
      return &pages_[i];
    }
  }

  // P not found in page table
  int freeFrameId = -1;
  if (free_list_.size() > 0) {
    // pick a free page from the free list
    freeFrameId = free_list_.front();
    free_list_.pop_front();
  } else {
    // first check if all pages in the buffer pool are pinned
    bool allPinned = true;
    for (auto i = 0; i < (int)pool_size_; i++) {
      if (pages_[i].GetPinCount() != 0) {
        allPinned = false;
        break;
      }
    }
    // all pinned, no victim can be found
    if (allPinned) {
      latch_.unlock();
      return nullptr;
    }

    // can find one victim from the CR
    frame_id_t* victimIdxPtr = nullptr;
    replacer_->Victim(victimIdxPtr);
    int victimIdx = *victimIdxPtr;
    victimIdxPtr = nullptr;
    // flush the victim frame
    FlushPageImplWithoutLock(pages_[victimIdx].page_id_);

    // check if the pageMap contains the victim frame Id

//    if (!pageMap.count(victimIdx)) {
//      pageMap[(frame_id_t)victimIdx] = new Page{};
//    }
    freeFrameId = victimIdx;
  }

  // Todo: how to insert P? Pin?
  // TODO: ?????????????
  // update P's metadata
//  pageMap[freeFrameId]->page_id_ = page_id;
  pages_[freeFrameId].page_id_ = page_id;
  disk_manager_->ReadPage(page_id, pages_[freeFrameId].GetData());

  std::cout<< "Our stupid fetch page result data is: " << pages_[page_id].GetData() << std::endl;

  latch_.unlock();
//  return pageMap[freeFrameId];
  return &pages_[freeFrameId];
}

bool BufferPoolManager::UnpinPageImpl(page_id_t page_id, bool is_dirty) {
  latch_.lock();
  bool res = false;
  int targetPageIdx = FindTargetPageIdx(page_id);
  if (targetPageIdx == -1) {
    // If the page was not found
    throw std::invalid_argument("The given page id was not found for UnPin function!");
  }

  if (pages_[targetPageIdx].GetPinCount() > 0) {
    // TODO: ??????????
    pages_[targetPageIdx].is_dirty_ = is_dirty;
    pages_[targetPageIdx].pin_count_--;
    if (pages_[targetPageIdx].pin_count_ == 0) {
      replacer_->Unpin(targetPageIdx);
    }
    res = true;
  }
  latch_.unlock();
  return res;
}

bool BufferPoolManager::FlushPageImplWithoutLock(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
  // Check if page id is valid
  bool res = false;
  if (page_id == INVALID_PAGE_ID) {
    throw std::invalid_argument("The given page id cannot be INVALID_PAGE_ID!");
  }
  int targetPageIdx = FindTargetPageIdx(page_id);
  if (targetPageIdx != -1) {
    // If the page was found
    if (pages_[targetPageIdx].is_dirty_) {
      disk_manager_->WritePage(page_id,pages_[targetPageIdx].GetData());
    }
    DeletePageImplWithoutLock(page_id);
    res = true;
  }
  return res;
}

bool BufferPoolManager::FlushPageImpl(page_id_t page_id) {
  // Make sure you call DiskManager::WritePage!
  // Check if page id is valid
  latch_.lock();
  bool res = FlushPageImplWithoutLock(page_id);
  latch_.unlock();
  return res;
}

Page *BufferPoolManager::NewPageImpl(page_id_t *page_id) {
  // 0.   Make sure you call DiskManager::AllocatePage!
  // 1.   If all the pages in the buffer pool are pinned, return nullptr.
  // 2.   Pick a victim page P from either the free list or the replacer. Always pick from the free list first.
  // 3.   Update P's metadata, zero out memory and add P to the page table.
  // 4.   Set the page ID output parameter. Return a pointer to P.
  latch_.lock();

  // ----------- cql's code starts here --------------------
  frame_id_t freeFrameId = INVALID_PAGE_ID;
  if (free_list_.size() > 0) {
    // pick a free page from the free list
    freeFrameId = free_list_.front();
    free_list_.pop_front();
  } else {
    if (!replacer_->Victim(&freeFrameId)) {
      // can't find any page from neither free list nor replacer
      latch_.unlock();
      return nullptr;
    }
    // free list is empty, but found one victim from replacer
    page_id_t pageIdToDelete = pages_[freeFrameId].GetPageId();
    // is page from replacer is dirty, write back to disk before delete (flush)
    if (pages_[freeFrameId].is_dirty_) {
      FlushPageImplWithoutLock(pageIdToDelete);
    }
    page_table_.erase(pageIdToDelete);
  }

  // get the free frame id

  *page_id = disk_manager_->AllocatePage();
  pages_[freeFrameId].ResetMemory();
  pages_[freeFrameId].page_id_ = *page_id;
  pages_[freeFrameId].pin_count_ = 1;
  // todo: pages_[freeFrameId].is_dirty_ = true; //???
  page_table_.insert({*page_id, freeFrameId});

  latch_.unlock();
  return &pages_[freeFrameId];
}
  // ----------- cql's code ends here ----------------------




  /**
   * --------------------------------------------------------
  int freeFrameId = -1;
  if (free_list_.size() > 0) {
    // pick a free page from the free list
    freeFrameId = free_list_.front();
    free_list_.pop_front();
  } else {
    // first check if all pages in the buffer pool are pinned
    bool allPinned = true;
    for (auto i = 0; i < (int)pool_size_; i++) {
      if (pages_[i].GetPinCount() == 0) {
        allPinned = false;
        break;
      }
    }
    // all pinned, no victim can be found

//    std::cout << "ALL PINNED = " << allPinned << std::endl;

    if (allPinned) {

      latch_.unlock();
      return nullptr;
    }

    // cannot find victim from CR bc CR's empty
    if (replacer_->Size() == 0) {
      latch_.unlock();
      return nullptr;
    }

    // can find one victim from the CR
    frame_id_t* victimIdxPtr = nullptr;

    std::cout << "Started VICTIM FUNCTION" << std::endl;

    replacer_->Victim(victimIdxPtr);
    int victimIdx = *victimIdxPtr;
    victimIdxPtr = nullptr;
    // flush the victim frame

    std::cout << "Passed VICTIM FUNCTION" << std::endl;

    FlushPageImplWithoutLock(pages_[victimIdx].page_id_);

    freeFrameId = victimIdx;
  }

//  // check if the pageMap contains the victim frame Id
//  if (!pageMap.count(freeFrameId)) {
//    pageMap[(frame_id_t)freeFrameId] = new Page{};
//  }

  // TODO: update P's metadata, zero out memory
  // update P's metadata
//  char empty_data[0];
//  std::memcpy(pageMap[freeFrameId]->GetData(), empty_data, PAGE_SIZE);
//  std::memcpy(pageMap[freeFrameId]->GetData(), empty_data, PAGE_SIZE);
  std::memset(pages_[freeFrameId].GetData(), 0, PAGE_SIZE);
//  pageMap[freeFrameId]->page_id_ = freeFrameId;
  pages_[freeFrameId].page_id_ = freeFrameId;
  pages_[freeFrameId].pin_count_++;



  *page_id = freeFrameId;

  latch_.unlock();
//  return pageMap[freeFrameId];
  return &pages_[freeFrameId];
}
   --------------------------------------------------------
   */




bool BufferPoolManager::DeletePageImplWithoutLock(page_id_t page_id) {
  int targetPageIdx = FindTargetPageIdx(page_id);
  if(targetPageIdx==-1) {
    return true;
  }
  if(pages_[targetPageIdx].pin_count_>0) {
    return false;
  }
  // P can be deleted
  disk_manager_->DeallocatePage(page_id);
  free_list_.emplace_back(targetPageIdx); // add the frame id back to free_list
  pages_[targetPageIdx].page_id_ = INVALID_PAGE_ID; // reset metadata
  return true;
}
bool BufferPoolManager::DeletePageImpl(page_id_t page_id) {
  // 0.   Make sure you call DiskManager::DeallocatePage!
  // 1.   Search the page table for the requested page (P).
  // 1.   If P does not exist, return true.
  // 2.   If P exists, but has a non-zero pin-count, return false. Someone is using the page.
  // 3.   Otherwise, P can be deleted. Remove P from the page table, reset its metadata and return it to the free list.
  latch_.lock();
  bool res = DeletePageImplWithoutLock(page_id);
  latch_.unlock();
  return res;
}


void BufferPoolManager::FlushAllPagesImpl() {
  // You can do it!
  latch_.lock();
  for(int i=0; i<(int)pool_size_; i++) {
    page_id_t page_id = pages_[i].page_id_;
    // skip invalid page_id
    if (page_id == INVALID_PAGE_ID) {
      continue;
    }
    FlushPageImplWithoutLock(page_id);
  }
  latch_.unlock();
}

// Helper functions below:
int BufferPoolManager::FindTargetPageIdx(page_id_t targetId) {
  for (int i = 0; i < (int)pool_size_; ++i) {
//    std::cout << "Current page id is " << pages_[i].page_id_ << std::endl;
    if (pages_[i].page_id_ != INVALID_PAGE_ID && pages_[i].page_id_ == targetId) {
      return i;
    }
  }
  return -1;
}


}  // namespace bustub
