//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// channel.h
//
// Identification: src/include/common/channel.h
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <condition_variable>  // NOLINT
#include <mutex>               // NOLINT
#include <queue>
#include <utility>
#include "readerwriterqueue/readerwriterqueue.h"

namespace bustub {

/**
 * Channels allow for safe sharing of data between threads.
 */
template <class T>
class Channel {
 public:
  Channel() = default;
  ~Channel() = default;

  /**
   * @brief Inserts an element into a shared queue.
   *
   * @param element The element to be inserted.
   */
  void Put(T element) { q_.enqueue(std::move(element)); }

  /**
   * @brief Gets an element from the shared queue. If the queue is empty, blocks until an element is available.
   */
  auto Get() -> T {
    T x;
    q_.wait_dequeue(x);
    return x;
  }

 private:
  moodycamel::BlockingReaderWriterQueue<T> q_;
};
}  // namespace bustub
