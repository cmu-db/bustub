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
#include <iostream>
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
  size = 0;
  // 计算需要多少个uint64_t
  refs = std::make_unique<uint64_t[]>((num_pages + UINT64_BITS - 1) / UINT64_BITS);
  in = std::make_unique<uint64_t[]>((num_pages + UINT64_BITS - 1) / UINT64_BITS);
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) {
  // victim当有frame可驱逐的时候返回其id,true
  // 否则返回false
  if (size == 0) {
    return false;
  }
  // 找到in = 1 AND refs == 0
  while (true) {
    // 只能驱逐在里面的
    if (get_bit(in, current_id)) {
      if (get_bit(refs, current_id)) {
        unset_bit(refs, current_id);
      } else {
        unset_bit(in, current_id);
        size--;
        *frame_id = current_id;
        // std::cout << current_id << " out\n";
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
  if (!get_bit(in, frame_id)) {
    return;
  }
  // pin_count[frame_id]++;
  unset_bit(refs, frame_id);
  unset_bit(in, frame_id);
  size--;
  // std::cout << frame_id << " out\n";
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
  if (frame_id < 0 || frame_id >= int32_t(max_pages)) {
    return;
  }
  // 将frame加入到这里面
  if (!get_bit(in, frame_id)) {
    size++;
  }
  set_bit(refs, frame_id);
  set_bit(in, frame_id);
  // std::cout << frame_id << " in\n";
}

size_t ClockReplacer::Size() { return size; }
}  // namespace bustub
