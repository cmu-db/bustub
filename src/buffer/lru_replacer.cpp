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

LRUReplacer::LRUReplacer(size_t num_pages) {}

LRUReplacer::~LRUReplacer() = default;

auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool { return false; }

void LRUReplacer::Pin(frame_id_t frame_id) {}

void LRUReplacer::Unpin(frame_id_t frame_id) {}

auto LRUReplacer::Size() -> size_t { return 0; }

}  // namespace bustub
