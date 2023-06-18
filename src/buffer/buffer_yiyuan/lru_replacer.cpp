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
#include <cassert>

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) : timestamp(1) {}

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  if (frame_map.empty()) {
    *frame_id = INVALID_FRAME_ID;
    return false;
  }

  replacer_mutex.lock();

  int64_t min_timestamp = frame_map.begin()->second;
  frame_id_t min_frame_id = frame_map.begin()->first;

  for (const auto &p : frame_map) {
    if (p.second < min_timestamp) {
      min_timestamp = p.second;
      min_frame_id = p.first;
    }
  }

  auto ret = frame_map.erase(min_frame_id);
  assert(ret > 0);

  *frame_id = min_frame_id;

  replacer_mutex.unlock();

  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  replacer_mutex.lock();

  frame_map.erase(frame_id);

  replacer_mutex.unlock();
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  replacer_mutex.lock();

  frame_map.insert({frame_id, timestamp++});

  replacer_mutex.unlock();
}

size_t LRUReplacer::Size() { return frame_map.size(); }

}  // namespace bustub
