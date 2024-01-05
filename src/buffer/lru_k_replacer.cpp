//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include "common/exception.h"

namespace bustub {

LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  std::lock_guard<std::mutex> lock(latch_);

  if (node_store_.empty()) {
    return false;
  }

  // Find the frame with the largest backward k-distance
  size_t max_distance = std::numeric_limits<size_t>::min();
  size_t distance_latest_max = std::numeric_limits<size_t>::min();
  frame_id_t victim_frame = INVALID_PAGE_ID;

  for (const auto &entry : node_store_) {
    const LRUKNode &node = entry.second;
    if (node.is_evictable_) {
      if (node.history_.size() < k_) {
        // Frame with less than k historical references, treat it as having +inf backward k-distance
        max_distance = std::numeric_limits<size_t>::max();
        size_t distance_latest = current_timestamp_ - node.history_.front();
        if(distance_latest_max <= distance_latest){
          distance_latest_max = distance_latest;
          victim_frame = entry.first;
        }
      } else {
        size_t distance = current_timestamp_ - node.history_.front();
        if (distance > max_distance) {
          max_distance = distance;
          victim_frame = entry.first;
        }
      }
    }
  }

  if (victim_frame != INVALID_PAGE_ID) {
    node_store_.erase(victim_frame);
    *frame_id = victim_frame;
    curr_size_--;
    replacer_size_++;
    return true;
  }

  return false;

}

void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
  std::lock_guard<std::mutex> lock(latch_);
  auto it = node_store_.find(frame_id);
  if(it == node_store_.end()){
    replacer_size_ --;
  }
  current_timestamp_++;
  node_store_[frame_id].history_.push_back(current_timestamp_);
  if (node_store_[frame_id].history_.size() > k_) {
    node_store_[frame_id].history_.pop_front();
  }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  std::lock_guard<std::mutex> lock(latch_);
  auto it = node_store_.find(frame_id);
  if (it != node_store_.end()) {
    if (it->second.is_evictable_ != set_evictable) {
      it->second.is_evictable_ = set_evictable;
      if (set_evictable) {
        curr_size_++;
      }else{
        curr_size_--;
      }
    }
  }else if(set_evictable){
    throw Exception("NO Frame Found!");
  }

}

void LRUKReplacer::Remove(frame_id_t frame_id) {
  std::lock_guard<std::mutex> lock(latch_);

  auto it = node_store_.find(frame_id);
  if(it !=node_store_.end()){
    if(it->second.is_evictable_){
      node_store_.erase(it);
      curr_size_--;
    }else{
      throw Exception("This frame is non-evictable");
    }
  }


}

auto LRUKReplacer::Size() -> size_t {
  std::lock_guard<std::mutex> lock(latch_);
  return curr_size_;
}

}  // namespace bustub
