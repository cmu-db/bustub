//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// clock_replacer.cpp
//
// Identification: src/buffer/clock_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/clock_replacer.h"

namespace bustub {

const size_t UINT64_BITS = 8 * sizeof(uint64_t);

// 获取uint64_t的某一位
bool get_bit(const std::unique_ptr<uint64_t[]> &arr, int32_t bit) {
  auto idx = bit / UINT64_BITS;
  bit %= UINT64_BITS;
  uint64_t val = arr[idx];
  val >>= size_t(bit);
  return (val & 1U) == 1;
}

// 设置1位为1
void set_bit(std::unique_ptr<uint64_t[]> &arr, int32_t bit) {
  auto idx = bit / UINT64_BITS;
  bit %= UINT64_BITS;
  uint64_t &val = arr[idx];
  val |= (1U << size_t(bit));
}
void unset_bit(std::unique_ptr<uint64_t[]> &arr, int32_t bit) {
  auto idx = bit / UINT64_BITS;
  bit %= UINT64_BITS;
  uint64_t &val = arr[idx];
  uint64_t one = 1;
  val &= ~(one << size_t(bit));
}

ClockReplacer::ClockReplacer(size_t num_pages) : max_pages(num_pages), current_id(0) {
  // 表示目前的页面都在pin着
  //  pins.resize(num_pages, 1);
  pin_count.resize(num_pages, 1);
  in.resize(num_pages, false);
  size = 0;
  //  size = max_pages;
  // 计算需要多少个uint64_t
  refs = std::make_unique<uint64_t[]>((num_pages + UINT64_BITS - 1) / UINT64_BITS);
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) {
  // victim当有frame可驱逐的时候返回其id,true
  // 否则返回false
  if (size == max_pages) {
    return false;
  }
  // 找到pin_count == 0 AND refs == 0
  while (true) {
    if (in[current_id]) {
      if (get_bit(refs, current_id)) {
        unset_bit(refs, current_id);
      } else {
        in[current_id] = false;
        size--;
        *frame_id = current_id;
        current_id++;
        current_id %= max_pages;
        return true;
      }
    }
    current_id++;
    current_id %= max_pages;
  }
  return false;
}

void ClockReplacer::Pin(frame_id_t frame_id) {
  if (frame_id < 0 || frame_id >= int32_t(max_pages)) {
    return;
  }
  if (!in[frame_id]) {
    return;
  }
  pin_count[frame_id]++;
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
  if (frame_id < 0 || frame_id >= int32_t(max_pages)) {
    return;
  }
  // 将frame加入到这里面
  set_bit(refs, frame_id);
  if (in[frame_id]) {
    set_bit(refs, frame_id);
  } else {
    size++;
  }
  in[frame_id] = true;
}

size_t ClockReplacer::Size() { return size; }
}  // namespace bustub
