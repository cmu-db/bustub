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

LRUKNode::LRUKNode(size_t k, frame_id_t fid) : k_(k), fid_(fid) {}

void LRUKNode::SetEvictable(bool evictable) { throw NotImplementedException("SetEvictable not implemented"); }

auto LRUKNode::IsEvictable() -> bool {
  throw NotImplementedException("IsEvictable not implemented");
  return false;
}

auto LRUKNode::GetFid() -> frame_id_t {
  throw NotImplementedException("GetFid not implemented");
  return 0;
}

auto LRUKNode::GetBackwardKDist(size_t current_timestamp) -> size_t {
  throw NotImplementedException("GetBackwardKDist not implemented");
  return 0;
}

auto LRUKNode::HasInfBackwardKDist() -> bool {
  throw NotImplementedException("HasInfBackwardKDist not implemented");
  return false;
}

auto LRUKNode::GetEarliestTimestamp() -> size_t {
  throw NotImplementedException("GetEarliestTimestamp not implemented");
  return 0;
}

void LRUKNode::InsertHistoryTimestamp(size_t current_timestamp) {
  throw NotImplementedException("InsertHistoryTimestamp not implemented");
}

LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool { return false; }

void LRUKReplacer::RecordAccess(frame_id_t frame_id) {}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {}

void LRUKReplacer::Remove(frame_id_t frame_id) {}

auto LRUKReplacer::Size() -> size_t { return 0; }

}  // namespace bustub
