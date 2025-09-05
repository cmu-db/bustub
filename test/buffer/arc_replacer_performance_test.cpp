//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// arc_replacer_performance_test.cpp
//
// Identification: test/buffer/arc_replacer_performance_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

/**
 * arc_replacer_performance_test.cpp
 */

#include <thread>  // NOLINT
#include "buffer/arc_replacer.h"

#include "gtest/gtest.h"

namespace bustub {

TEST(ArcReplacerPerformanceTest, RecordAccessPerformanceTest) {
  // Test RecordAccess performance
  std::cout << "BEGIN" << std::endl;
  std::cout << "This test will see how your RecordAccess performs when the list is large. " << std::endl;
  std::cout << "If this takes above 3s on average, "
               "you might get into trouble trying to get full score in some following projects... "
            << std::endl;
  std::cout << "if you care, you may want to think of "
               "what could be very slow when the list is very large, and how to make that faster"
            << std::endl;
  const size_t bpm_size = 256 << 10;  // 1GB
  ArcReplacer arc_replacer(bpm_size);
  // Fillup mfu with lots of pages
  for (size_t i = 0; i < bpm_size; i++) {
    arc_replacer.RecordAccess(i, i);
    arc_replacer.SetEvictable(i, true);
  }
  // Keep accessing pages in middle of the list
  std::vector<std::thread> threads;
  const size_t rounds = 10;
  size_t access_frame_id = 256 << 9;
  std::vector<size_t> access_times;
  for (size_t round = 0; round < rounds; round++) {
    auto start_time = std::chrono::system_clock::now();
    for (size_t i = 0; i < bpm_size; i++) {
      arc_replacer.RecordAccess(access_frame_id, access_frame_id);
      access_frame_id = (access_frame_id + 1) % bpm_size;
    }
    auto end_time = std::chrono::system_clock::now();
    access_times.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
  }
  double total = 0;
  for (const auto &x : access_times) {
    total += x;
  }
  total /= 1000;
  double avg = total / access_times.size();
  std::cout << "END" << std::endl;
  std::cout << "Average time used: " << avg << "s." << std::endl;
  std::cout << "If this takes above 3s on average, "
               "you might get into trouble trying to get full score in some following projects... "
            << std::endl;
  std::cout << "If you care, try optimizing RecordAccess for a bit" << std::endl;
  ASSERT_LT(avg, 3);
}

}  // namespace bustub
