//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// rwmutex.h
//
// Identification: src/include/common/rwmutex.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <climits>
#include <condition_variable>  // NOLINT
#include <mutex>               // NOLINT

#include "common/macros.h"

namespace bustub {

/**
 * RWMutex is a reader-writer lock backed by std::mutex.
 */
class RWMutex {
  using mutex_t = std::mutex;
  using cond_t = std::condition_variable;
  static const uint32_t max_readers_ = UINT_MAX;

 public:
  RWMutex() = default;
  ~RWMutex() { std::lock_guard<mutex_t> guard(mutex_); }

  DISALLOW_COPY(RWMutex);

  /**
   * Acquire a write lock.
   */
  void WLock() {
    std::unique_lock<mutex_t> lock(mutex_);
    while (writer_entered_) {
      reader_.wait(lock);
    }
    writer_entered_ = true;
    while (reader_count_ > 0) {
      writer_.wait(lock);
    }
  }

  /**
   * Release a write lock.
   */
  void WUnlock() {
    std::lock_guard<mutex_t> guard(mutex_);
    writer_entered_ = false;
    reader_.notify_all();
  }

  /**
   * Acquire a read lock.
   */
  void RLock() {
    std::unique_lock<mutex_t> lock(mutex_);
    while (writer_entered_ || reader_count_ == max_readers_) {
      reader_.wait(lock);
    }
    reader_count_++;
  }

  /**
   * Release a read lock.
   */
  void RUnlock() {
    std::lock_guard<mutex_t> guard(mutex_);
    reader_count_--;
    if (writer_entered_) {
      if (reader_count_ == 0) {
        writer_.notify_one();
      }
    } else {
      if (reader_count_ == max_readers_ - 1) {
        reader_.notify_one();
      }
    }
  }

 private:
  mutex_t mutex_;
  cond_t writer_;
  cond_t reader_;
  uint32_t reader_count_{0};
  bool writer_entered_{false};
};

}  // namespace bustub
