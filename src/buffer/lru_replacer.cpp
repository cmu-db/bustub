//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) {
  capacity_ = num_pages;
}

LRUReplacer::~LRUReplacer() = default;

auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
  // get latch
  latch_.lock();

  if(lru_list_.empty()){
    // no pages, unlock and return
    latch_.unlock();
    return false;
  }
  frame_id_t lru_frame = lru_list_.back();
  lru_map_.erase(lru_frame);
  *frame_id = lru_frame;
  lru_list_.pop_back();

  latch_.unlock();
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  // get latch
  latch_.lock();

  if(lru_map_.count(frame_id) != 0){
    lru_list_.erase(lru_map_[frame_id]);
    lru_map_.erase(frame_id);
  }
  // release the latch
  latch_.unlock();
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  // get latch
  latch_.lock();

  // not exists, insert new frame to list head
  if(lru_map_.count(frame_id) != 0) {
    latch_.unlock();
    return;
  }
  while(Size() >= capacity_) {
    // drop frame before insert if exceeded capacity
    frame_id_t last = lru_list_.back();
    lru_list_.pop_back();
    lru_map_.erase(last);
  }
  // insert and release lock
  lru_list_.push_front(frame_id);
  lru_map_[frame_id] = lru_list_.begin();

  latch_.unlock();
}

auto LRUReplacer::Size() -> size_t {
  return lru_list_.size();
}
}  // namespace bustub
