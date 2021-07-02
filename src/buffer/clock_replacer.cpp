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

ClockReplacer::ClockReplacer(size_t num_pages) {
  capacity = num_pages;
  totalInCR = 0;
  clockHand = 0;

  for (auto i = 0; i < (int)num_pages; i++) {
    replacerFrameList.push_back(clockReplacer_myframe{false, false});
  }
}

ClockReplacer::~ClockReplacer() = default;

bool ClockReplacer::Victim(frame_id_t *frame_id) {
  crMutex.lock();
  int pos = clockHand;
  bool found = false;
  for(int i=0;i<capacity;i++){
    pos = (clockHand+i)%capacity;
    if(replacerFrameList.at(pos).isFrameInCR()){
      if(replacerFrameList.at(pos).getRefFlag()) {
        replacerFrameList.at(pos).updateRefFlag(false);
      }
      else {
        found = true;
        break;
      }
    }
  }
  if(!found){
    for (int i = 0; i < capacity; ++i) {
      if(replacerFrameList.at(i).isFrameInCR()) {
        found = true;
        pos = i;
        break;
      }
    }
  }
  if(found){
//    *frame_id = pos + 1; // plus one to match the 1-indexed frame_id
    *frame_id = pos;
    clockHand = (pos + 1) % capacity;
    // remove the frame from buffer pool & clock replacer
    replacerFrameList.at(pos).updateIsInCR(false); // remove the frame
    totalInCR--;
  }
  crMutex.unlock();
  return found;
}

void ClockReplacer::Pin(frame_id_t frame_id) {
  crMutex.lock();
  if(replacerFrameList.at(frame_id).isFrameInCR()){
    replacerFrameList.at(frame_id).updateIsInCR(false);
    totalInCR--;
  }
  crMutex.unlock();
}

void ClockReplacer::Unpin(frame_id_t frame_id) {
  crMutex.lock();
//  int pos = (frame_id - 1) % capacity;
  if (!replacerFrameList.at(frame_id).isFrameInCR()) {
    replacerFrameList.at(frame_id).updateIsInCR(true);
    replacerFrameList.at(frame_id).updateRefFlag(true);
    totalInCR++;
  }

  crMutex.unlock();
}

size_t ClockReplacer::Size() {
  crMutex.lock();
  auto toReturn = totalInCR;
  crMutex.unlock();
  return toReturn;
}

}  // namespace bustub
