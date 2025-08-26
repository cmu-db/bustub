//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// count_min_sketch_test.cpp
//
// Identification: test/primer/count_min_sketch_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <chrono>  // NOLINT
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <thread>  // NOLINT
#include <vector>

#include "gtest/gtest.h"
#include "primer/count_min_sketch.h"

namespace bustub {

TEST(CountMinSketchTest, BasicTest1) {
  // Test basic insertion and counting with strings
  auto cms = CountMinSketch<std::string>(200, 12);

  ASSERT_EQ(cms.Count("test"), 0);

  // Initial insertion and verification
  cms.Insert("Welcome to CMU DB (15-445/645)");
  ASSERT_EQ(cms.Count("Welcome to CMU DB (15-445/645)"), 1);
  for (uint64_t i = 0; i < 10; i++) {
    cms.Insert("DJ-Cache");
    cms.Insert("Sirui");
    cms.Insert("Andy");
    cms.Insert("Melody");
    cms.Insert("William");
    cms.Insert("Saransh");
    cms.Insert("Song");
    cms.Insert("Ruiqi");
    cms.Insert("David");

    int curr_count = i + 1;
    ASSERT_EQ(cms.Count("DJ-Cache"), curr_count);
    ASSERT_EQ(cms.Count("Sirui"), curr_count);
    ASSERT_EQ(cms.Count("Andy"), curr_count);
    ASSERT_EQ(cms.Count("Melody"), curr_count);
    ASSERT_EQ(cms.Count("William"), curr_count);
    ASSERT_EQ(cms.Count("Saransh"), curr_count);
    ASSERT_EQ(cms.Count("Song"), curr_count);
    ASSERT_EQ(cms.Count("Ruiqi"), curr_count);
    ASSERT_EQ(cms.Count("David"), curr_count);
  }

  // Verify final counts after all iterations
  ASSERT_EQ(cms.Count("DJ-Cache"), 10);
  ASSERT_EQ(cms.Count("Sirui"), 10);
  ASSERT_EQ(cms.Count("Andy"), 10);
  ASSERT_EQ(cms.Count("Melody"), 10);
  ASSERT_EQ(cms.Count("William"), 10);
  ASSERT_EQ(cms.Count("Saransh"), 10);
  ASSERT_EQ(cms.Count("Song"), 10);
  ASSERT_EQ(cms.Count("Ruiqi"), 10);
  ASSERT_EQ(cms.Count("David"), 10);
  ASSERT_EQ(cms.Count("Welcome to CMU DB (15-445/645)"), 1);
  ASSERT_EQ(cms.Count("NonExistent"), 0);
}

TEST(CountMinSketchTest, BasicTest2) {
  // Test multiple items with different counts and numeric values
  auto cms = CountMinSketch<int64_t>(500, 20);

  ASSERT_EQ(cms.Count(0), 0);
  cms.Insert(0);
  ASSERT_EQ(cms.Count(0), 1);

  for (uint64_t i = 0; i < 30; i++) {
    // First batch of insertions
    for (uint64_t j = 0; j < 4; j++) {
      cms.Insert(10);
      cms.Insert(122);
      cms.Insert(200);
      cms.Insert(911);
      cms.Insert(15445);

      int curr_count = (i * 4) + j + 1;
      ASSERT_EQ(cms.Count(10), curr_count);
      ASSERT_EQ(cms.Count(122), curr_count);
      ASSERT_EQ(cms.Count(200), curr_count);
      ASSERT_EQ(cms.Count(911), curr_count);
      ASSERT_EQ(cms.Count(15445), curr_count);
    }

    // Second batch with negative numbers
    for (uint64_t j = 0; j < 5; j++) {
      cms.Insert(-1);
      cms.Insert(-2);
      cms.Insert(-3);
      cms.Insert(-15445);

      int curr_count = (i * 5) + j + 1;
      ASSERT_EQ(cms.Count(-1), curr_count);
      ASSERT_EQ(cms.Count(-2), curr_count);
      ASSERT_EQ(cms.Count(-3), curr_count);
      ASSERT_EQ(cms.Count(-15445), curr_count);
    }
  }

  // Verify final counts
  ASSERT_EQ(cms.Count(0), 1);
  ASSERT_EQ(cms.Count(10), 120);
  ASSERT_EQ(cms.Count(122), 120);
  ASSERT_EQ(cms.Count(200), 120);
  ASSERT_EQ(cms.Count(911), 120);
  ASSERT_EQ(cms.Count(15445), 120);
  ASSERT_EQ(cms.Count(-1), 150);
  ASSERT_EQ(cms.Count(-2), 150);
  ASSERT_EQ(cms.Count(-3), 150);
  ASSERT_EQ(cms.Count(-15445), 150);
  ASSERT_EQ(cms.Count(999999), 0);
}

TEST(CountMinSketchTest, EdgeTest1) {
  // Invalid construction parameters
  for (int i = 10; i < 50; i += 10) {
    ASSERT_THROW(CountMinSketch<int>(0, i), std::invalid_argument);
    ASSERT_THROW(CountMinSketch<int>(i * 5, 0), std::invalid_argument);
  }
}

TEST(CountMinSketchTest, EdgeTest2) {
  // Test count-min sketch behavior with minimum width and various depths
  // Test with width = 1 (all hash to the same bucket)
  auto cms_min_width = CountMinSketch<int64_t>(1, 20);

  cms_min_width.Insert(1);
  cms_min_width.Insert(2);
  ASSERT_EQ(cms_min_width.Count(1), 2);
  ASSERT_EQ(cms_min_width.Count(2), 2);
  cms_min_width.Insert(3);
  cms_min_width.Insert(4);
  // All items will show count of 4 because with width = 1, everything hashes to the same column
  ASSERT_EQ(cms_min_width.Count(1), 4);
  ASSERT_EQ(cms_min_width.Count(2), 4);
  ASSERT_EQ(cms_min_width.Count(3), 4);
  ASSERT_EQ(cms_min_width.Count(4), 4);

  // Test with depth = 1 (no collision mitigation)
  auto cms_min_depth = CountMinSketch<int64_t>(50, 1);

  cms_min_depth.Insert(15445);
  cms_min_depth.Insert(15445 + 4);
  cms_min_depth.Insert(15445);

  // Only one hash function, each bucket will have at least the count of the inserted items
  ASSERT_GE(cms_min_depth.Count(15445), 2);
  ASSERT_GE(cms_min_depth.Count(15445 + 4), 1);
  ASSERT_GE(cms_min_depth.Count(15445 + 8), 0);

  // Test with both minimum dimensions
  auto cms_min_both = CountMinSketch<int64_t>(1, 1);

  for (int i = 0; i < 5; i++) {
    cms_min_both.Insert(i);
  }

  for (int i = 0; i < 5; i++) {
    ASSERT_EQ(cms_min_both.Count(i), 5);
  }

  // Even items we never inserted will show count of 5 due to single bucket
  ASSERT_EQ(cms_min_both.Count(999), 5);
  ASSERT_EQ(cms_min_both.Count(-1), 5);
  ASSERT_EQ(cms_min_both.Count(15445), 5);
}

TEST(CountMinSketchTest, MoveTest) {
  // Test 1: Move regular count-min sketch
  auto cms1 = CountMinSketch<int>(100, 10);

  for (int i = 0; i < 5; i++) {
    cms1.Insert(1);
  }
  for (int i = 0; i < 3; i++) {
    cms1.Insert(2);
  }
  cms1.Insert(3);

  ASSERT_EQ(cms1.Count(1), 5);
  ASSERT_EQ(cms1.Count(2), 3);
  ASSERT_EQ(cms1.Count(3), 1);

  auto cms2 = std::move(cms1);

  ASSERT_EQ(cms2.Count(1), 5);
  ASSERT_EQ(cms2.Count(2), 3);
  ASSERT_EQ(cms2.Count(3), 1);

  // Test 2: Move with collision scenario (width = 1)
  auto cms3 = CountMinSketch<int>(1, 4);

  for (int i = 0; i < 3; i++) {
    cms3.Insert(i);
  }

  ASSERT_EQ(cms3.Count(0), 3);
  ASSERT_EQ(cms3.Count(1), 3);
  ASSERT_EQ(cms3.Count(2), 3);
  ASSERT_EQ(cms3.Count(999), 3);

  CountMinSketch<int> cms4(1, 4);
  cms4.Insert(42);
  ASSERT_EQ(cms4.Count(42), 1);

  cms4 = std::move(cms3);

  ASSERT_EQ(cms4.Count(4), 3);
  ASSERT_EQ(cms4.Count(5), 3);
  ASSERT_EQ(cms4.Count(6), 3);
  ASSERT_EQ(cms4.Count(998), 3);

  // Test 3: Move with minimum dimensions (depth = 1, width = 1)
  auto cms5 = CountMinSketch<int>(1, 1);

  for (int i = 0; i < 5; i++) {
    cms5.Insert(i);
  }

  ASSERT_EQ(cms5.Count(0), 5);
  ASSERT_EQ(cms5.Count(999), 5);

  auto cms6 = std::move(cms5);

  ASSERT_EQ(cms6.Count(0), 5);
  ASSERT_EQ(cms6.Count(1), 5);
  ASSERT_EQ(cms6.Count(999), 5);

  cms6.Insert(1000);

  ASSERT_EQ(cms6.Count(0), 6);
  ASSERT_EQ(cms6.Count(1000), 6);
  ASSERT_EQ(cms6.Count(-1), 6);
}

TEST(CountMinSketchTest, ClearTest) {
  // Test normal case clearing
  auto cms = CountMinSketch<int>(200, 10);

  for (int i = 0; i < 15; i++) {
    cms.Insert(1);
  }
  for (int i = 0; i < 10; i++) {
    cms.Insert(2);
  }
  for (int i = 0; i < 8; i++) {
    cms.Insert(3);
  }

  ASSERT_EQ(cms.Count(1), 15);
  ASSERT_EQ(cms.Count(2), 10);
  ASSERT_EQ(cms.Count(3), 8);
  ASSERT_EQ(cms.Count(999), 0);

  cms.Clear();

  ASSERT_EQ(cms.Count(1), 0);
  ASSERT_EQ(cms.Count(2), 0);
  ASSERT_EQ(cms.Count(3), 0);
  ASSERT_EQ(cms.Count(999), 0);
}

TEST(CountMinSketchTest, MergeTest) {
  // Test merging sketches with various configurations and collision scenarios

  // Test 1: Normal merge with no collisions
  auto cms1 = CountMinSketch<std::string>(250, 8);
  auto cms2 = CountMinSketch<std::string>(250, 8);

  for (int i = 0; i < 5; i++) {
    cms1.Insert("055");
  }
  for (int i = 0; i < 2; i++) {
    cms1.Insert("4987");
  }
  for (int i = 0; i < 3; i++) {
    cms1.Insert("3125");
  }
  cms1.Insert("2256");

  ASSERT_EQ(cms1.Count("055"), 5);
  ASSERT_EQ(cms1.Count("4987"), 2);
  ASSERT_EQ(cms1.Count("3125"), 3);
  ASSERT_EQ(cms1.Count("2256"), 1);

  for (int i = 0; i < 5; i++) {
    cms2.Insert("4739");
  }
  for (int i = 0; i < 2; i++) {
    cms2.Insert("3125");
  }
  for (int i = 0; i < 3; i++) {
    cms2.Insert("4987");
  }
  for (int i = 0; i < 4; i++) {
    cms2.Insert("2256");
  }

  ASSERT_EQ(cms2.Count("4739"), 5);
  ASSERT_EQ(cms2.Count("3125"), 2);
  ASSERT_EQ(cms2.Count("4987"), 3);
  ASSERT_EQ(cms2.Count("2256"), 4);

  cms1.Merge(cms2);

  ASSERT_EQ(cms1.Count("055"), 5);
  ASSERT_EQ(cms1.Count("4987"), 5);
  ASSERT_EQ(cms1.Count("4739"), 5);
  ASSERT_EQ(cms1.Count("2256"), 5);
  ASSERT_EQ(cms1.Count("3125"), 5);

  ASSERT_EQ(cms2.Count("4739"), 5);
  ASSERT_EQ(cms2.Count("3125"), 2);
  ASSERT_EQ(cms2.Count("4987"), 3);

  // Test 2: Merge with collision scenarios (width=1)
  auto cms3 = CountMinSketch<int>(1, 20);
  auto cms4 = CountMinSketch<int>(1, 20);

  cms3.Insert(1);
  cms3.Insert(2);
  cms3.Insert(5);

  cms4.Insert(3);
  cms4.Insert(4);

  ASSERT_EQ(cms3.Count(1), 3);
  ASSERT_EQ(cms3.Count(2), 3);
  ASSERT_EQ(cms3.Count(5), 3);
  ASSERT_EQ(cms4.Count(3), 2);
  ASSERT_EQ(cms4.Count(4), 2);

  cms3.Merge(cms4);

  ASSERT_EQ(cms3.Count(1), 5);
  ASSERT_EQ(cms3.Count(2), 5);
  ASSERT_EQ(cms3.Count(3), 5);
  ASSERT_EQ(cms3.Count(4), 5);
  ASSERT_EQ(cms3.Count(996), 5);
}

TEST(CountMinSketchTest, ParallelTest) {
  for (int iter = 500; iter <= 1000; iter += 100) {
    for (int num_threads = 8; num_threads <= 16; num_threads += 2) {
      // Test thread safety
      auto cms = CountMinSketch<std::string>(500, 15);

      std::vector<std::thread> threads;
      const int iterations = iter;
      const int threads_count = num_threads;
      threads.reserve(threads_count);

      // Create threads that concurrently insert items
      for (int i = 0; i < threads_count; i++) {
        threads.emplace_back([&cms, iterations]() {
          for (int j = 0; j < iterations; j++) {
            cms.Insert("frequent");
            if (j % 3 == 0) {
              cms.Insert("less_frequent");
            }
          }
        });
      }

      // Wait for all threads to complete
      for (auto &thread : threads) {
        thread.join();
      }

      auto top = cms.TopK(2, {"frequent", "less_frequent"});
      ASSERT_EQ(top.size(), 2);

      ASSERT_EQ(top[0].first, "frequent");
      ASSERT_EQ(top[0].second, num_threads * iterations);

      ASSERT_EQ(top[1].first, "less_frequent");
      int expected_less_count_iter = static_cast<int>(std::ceil(static_cast<double>(iterations) / 3));
      int expected_less_frequent_count = num_threads * expected_less_count_iter;
      ASSERT_EQ(top[1].second, expected_less_frequent_count);
    }
  }
}

TEST(CountMinSketchTest, ComplexParallelTest) {
  for (int iter = 200; iter <= 500; iter += 100) {
    auto cms1 = CountMinSketch<int>(1000, 10);
    auto cms2 = CountMinSketch<int>(1000, 10);

    const int num_threads = 8;
    const int iterations = iter;

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; i++) {
      threads.emplace_back([&cms1, iterations, i]() {
        for (int j = 0; j < iterations; j++) {
          cms1.Insert(i);
          cms1.Insert(42);
          if (j % 2 == 0) {
            cms1.Insert(100);
          }
        }
      });
    }

    std::vector<std::thread> threads2;
    threads2.reserve(num_threads / 2);
    for (int i = 0; i < num_threads / 2; i++) {
      threads2.emplace_back([&cms2, iterations, i]() {
        for (int j = 0; j < iterations / 2; j++) {
          cms2.Insert(i + 100);
          cms2.Insert(42);
          if (j % 3 == 0) {
            cms2.Insert(200);
          }
        }
      });
    }

    for (auto &thread : threads) {
      thread.join();
    }
    for (auto &thread : threads2) {
      thread.join();
    }

    // Check cms1 count
    for (int i = 0; i < num_threads; i++) {
      if (i != 42 && i != 100) {
        ASSERT_EQ(cms1.Count(i), iterations);
      }
    }

    int expected_42_count = num_threads * iterations;
    ASSERT_EQ(cms1.Count(42), expected_42_count);

    int expected_100_count = num_threads * (iterations / 2);
    ASSERT_EQ(cms1.Count(100), expected_100_count);

    cms1.Clear();
    for (int i = 0; i < iterations; i++) {
      ASSERT_EQ(cms1.Count(i), 0);
    }
    ASSERT_EQ(cms1.Count(42), 0);
    ASSERT_EQ(cms1.Count(100), 0);

    // Check cms2 count
    for (int i = 0; i < num_threads / 2; i++) {
      ASSERT_EQ(cms2.Count(i + 100), iterations / 2);
    }

    int expected_42_count_2 = (num_threads / 2) * (iterations / 2);
    ASSERT_EQ(cms2.Count(42), expected_42_count_2);

    int expected_200_count_iter = static_cast<int>(std::ceil(static_cast<double>(iterations) / 2.0 / 3.0));
    int expected_200_count = (num_threads / 2) * expected_200_count_iter;
    ASSERT_EQ(cms2.Count(200), expected_200_count);

    // Insert a unique item into cms1 and check merged
    cms1.Insert(100);
    cms1.Merge(cms2);

    ASSERT_GT(cms1.Count(42), 0);
    ASSERT_EQ(cms1.Count(100), iterations / 2 + 1);

    ASSERT_GT(cms2.Count(42), 0);
  }
}

TEST(CountMinSketchTest, TopKBasicTest) {
  // Test basic top-k functionality
  for (int iter = 1; iter < 10; iter++) {
    auto cms = CountMinSketch<std::string>(10, 3);

    for (int i = 0; i < iter + 4; i++) {
      cms.Insert("frequent");
    }
    for (int i = 0; i < iter + 2; i++) {
      cms.Insert("medium");
    }
    for (int i = 0; i < iter; i++) {
      cms.Insert("rare");
    }

    auto top = cms.TopK(3, {"frequent", "medium", "rare"});
    ASSERT_EQ(top.size(), 3);
    ASSERT_EQ(top[0].first, "frequent");
    ASSERT_EQ(top[0].second, iter + 4);
    ASSERT_EQ(top[1].first, "medium");
    ASSERT_EQ(top[1].second, iter + 2);
    ASSERT_EQ(top[2].first, "rare");
    ASSERT_EQ(top[2].second, iter);
  }
}

TEST(CountMinSketchTest, TopKDynamicTest) {
  // Test dynamic updates to top-k
  const std::vector<std::vector<int>> test_cases = {
      {1, 2, 3, 4}, {7, 5, 3, 1}, {2, 2, 5, 7}, {6, 6, 2, 2}, {1, 3, 6, 6}, {400, 200, 300, 100},
  };

  const std::vector<std::vector<int>> expected_orders = {{4, 3, 2}, {1, 2, 3}, {4, 3, 1},
                                                         {1, 2, 4}, {4, 3, 2}, {1, 3, 2}};

  auto cms = CountMinSketch<int>(200, 15);

  for (size_t idx = 0; idx < test_cases.size(); idx++) {
    const auto &case_vec = test_cases[idx];
    for (int i = 1; i <= 4; i++) {
      for (int cnt = 0; cnt < case_vec[i - 1]; cnt++) {
        cms.Insert(i);
      }
    }

    auto topk = cms.TopK(3, {1, 2, 3, 4});
    ASSERT_EQ(topk.size(), 3);
    for (size_t rank = 0; rank < topk.size(); rank++) {
      ASSERT_EQ(topk[rank].first, expected_orders[idx][rank]);
    }
  }
}

TEST(CountMinSketchTest, TopKComprehensiveTest) {
  // Test comprehensive TopK functionality with parallel insertions and merging
  const int width = random() % 200 + 100;
  const int depth = random() % 8 + 8;
  const int k = 5;

  auto cms1 = CountMinSketch<std::string>(width, depth);
  auto cms2 = CountMinSketch<std::string>(width, depth);
  auto cms3 = CountMinSketch<std::string>(width, depth);
  auto cms4 = CountMinSketch<std::string>(width, depth);

  const std::vector<std::string> songs1 = {"C.R.E.A.M.",
                                           "Protect Ya Neck",
                                           "Method Man",
                                           "Bring da Ruckus",
                                           "Da Mystery of Chessboxin'",
                                           "Can It Be All So Simple",
                                           "Wu-Tang Clan Ain't Nuthing ta F' Wit"};

  const std::vector<std::string> songs2 = {"Triumph",   "Gravel Pit",      "Tearz",     "C.R.E.A.M.",
                                           "Ice Cream", "Protect Ya Neck", "Method Man"};

  const std::vector<std::string> all_songs = {"C.R.E.A.M.",
                                              "Protect Ya Neck",
                                              "Method Man",
                                              "Bring da Ruckus",
                                              "Da Mystery of Chessboxin'",
                                              "Can It Be All So Simple",
                                              "Wu-Tang Clan Ain't Nuthing ta F' Wit",
                                              "Triumph",
                                              "Gravel Pit",
                                              "Ice Cream",
                                              "Tearz"};

  std::vector<std::thread> threads1;
  std::vector<std::thread> threads2;
  const int num_threads1 = 12;
  const int num_threads2 = 8;
  const int iterations = 1000;
  threads1.reserve(num_threads1);
  threads2.reserve(num_threads2);

  // Create threads for first sketch
  for (int i = 0; i < num_threads1; i++) {
    threads1.emplace_back([&cms1, &cms2, &cms3, &cms4, &songs1, &songs2]() {
      for (int j = 0; j < iterations; j++) {
        // Insert into cms1
        cms1.Insert(songs1[0]);  // 1000 * 12 = 12000 "C.R.E.A.M."
        if (j % 2 == 0) {
          cms1.Insert(songs1[1]);  // 500 * 12 = 6000 "Protect Ya Neck"
        }

        // Insert into cms3
        if (j % 3 == 0) {
          cms3.Insert(songs1[2]);  // 334 * 12 = 4008 "Method Man"
        }
        if (j % 4 == 0) {
          cms3.Insert(songs1[3]);  // 250 * 12 = 3000 "Bring da Ruckus"
        }

        // Insert into cms2
        if (j % 6 == 0) {
          cms2.Insert(songs2[4]);  // 167 * 12 = 2004 "Ice Cream"
        }
        if (j % 12 == 0) {
          cms2.Insert(songs2[5]);  // 84 * 12 = 1008 "Protect Ya Neck"
        }

        // Insert into cms4
        if (j % 15 == 0) {
          cms4.Insert(songs2[6]);  // 67 * 12 = 804 "Method Man"
        }
      }
    });
  }

  // Create threads for second sketch
  for (int i = 0; i < num_threads2; i++) {
    threads2.emplace_back([&cms2, &songs2, &cms1, &songs1]() {
      for (int j = 0; j < iterations; j++) {
        // Insert into csm1
        cms2.Insert(songs2[0]);  // 1000 * 8 = 8000 "Triumph"
        if (j % 2 == 0) {
          cms2.Insert(songs2[1]);  // 500 * 8 = 4000 "Gravel Pit"
        }
        if (j % 4 == 0) {
          cms2.Insert(songs2[2]);  // 250 * 8 = 2000 "Tearz"
        }
        if (j % 5 == 0) {
          cms2.Insert(songs2[3]);  // 200 * 8 = 1600 "C.R.E.A.M."
        }

        // Insert into ***cms1***
        if (j % 5 == 0) {
          cms1.Insert(songs1[4]);  // 200 * 8 = 1600 "Da Mystery of Chessboxin'"
        }
        if (j % 10 == 0) {
          cms1.Insert(songs1[5]);  // 100 * 8 = 800 "Can It Be All So Simple"
        }
        if (j % 20 == 0) {
          cms1.Insert(songs1[6]);  // 50 * 8 = 400 "Wu-Tang Clan Ain't Nuthing ta F' Wit"
        }
      }
    });
  }

  // Total results
  // "C.R.E.A.M." 12000 + 1600 = 13600
  // "Protect Ya Neck" 6000 + 1008 = 7008
  // "Method Man" 4008 + 804 = 4812
  // "Bring da Ruckus" 3000 + 0 = 3000
  // "Da Mystery of Chessboxin'" 0 + 1600 = 1600
  // "Can It Be All So Simple" 0 + 800 = 800
  // "Wu-Tang Clan Ain't Nuthing ta F' Wit" 0 + 400 = 400
  // "Triumph" 0 + 8000 = 8000
  // "Gravel Pit" 0 + 4000 = 4000
  // "Ice Cream" 2004 + 0 = 2004
  // "Tearz" 0 + 2000 = 2000

  for (auto &thread : threads1) {
    thread.join();
  }
  for (auto &thread : threads2) {
    thread.join();
  }

  cms1.Merge(cms3);
  cms2.Merge(cms4);

  auto top1 = cms1.TopK(k, songs1);
  ASSERT_EQ(top1.size(), k);
  ASSERT_EQ(top1[0].first, songs1[0]);
  ASSERT_EQ(top1[1].first, songs1[1]);
  ASSERT_EQ(top1[2].first, songs1[2]);
  ASSERT_EQ(top1[3].first, songs1[3]);
  ASSERT_EQ(top1[4].first, songs1[4]);

  ASSERT_EQ(top1[0].second, num_threads1 * iterations);
  ASSERT_EQ(top1[1].second, num_threads1 * (iterations / 2));
  ASSERT_EQ(top1[2].second, num_threads1 * (iterations / 3 + 1));
  ASSERT_EQ(top1[3].second, num_threads1 * (iterations / 4));
  ASSERT_EQ(top1[4].second, num_threads2 * (iterations / 5));

  auto top2 = cms2.TopK(k, songs2);
  ASSERT_EQ(top2.size(), k);
  ASSERT_EQ(top2[0].first, songs2[0]);
  ASSERT_EQ(top2[1].first, songs2[1]);
  ASSERT_EQ(top2[2].first, songs2[4]);
  ASSERT_EQ(top2[3].first, songs2[2]);
  ASSERT_EQ(top2[4].first, songs2[3]);

  ASSERT_EQ(top2[0].second, num_threads2 * iterations);
  ASSERT_EQ(top2[1].second, num_threads2 * (iterations / 2));
  ASSERT_EQ(top2[2].second, num_threads1 * (iterations / 6 + 1));
  ASSERT_EQ(top2[3].second, num_threads2 * (iterations / 4));
  ASSERT_EQ(top2[4].second, num_threads2 * (iterations / 5));

  // Merge the sketches
  cms1.Merge(cms2);

  auto top_merged = cms1.TopK(k, all_songs);
  ASSERT_EQ(top_merged.size(), k);

  ASSERT_EQ(top_merged[0].first, "C.R.E.A.M.");
  ASSERT_EQ(top_merged[1].first, "Triumph");
  ASSERT_EQ(top_merged[2].first, "Protect Ya Neck");
  ASSERT_EQ(top_merged[3].first, "Method Man");
  ASSERT_EQ(top_merged[4].first, "Gravel Pit");

  // Verify merged counts combine correctly
  ASSERT_EQ(top_merged[0].second, (num_threads1 * iterations) + (num_threads2 * (iterations / 5)));
  ASSERT_EQ(top_merged[1].second, (num_threads2 * iterations));
  ASSERT_EQ(top_merged[2].second, (num_threads1 * (iterations / 2)) + (num_threads1 * (iterations / 12 + 1)));
  ASSERT_EQ(top_merged[3].second, (num_threads1 * (iterations / 3 + 1)) + (num_threads1 * (iterations / 15 + 1)));
  ASSERT_EQ(top_merged[4].second, (num_threads2 * (iterations / 2)));
}

TEST(CountMinSketchTest, ContentionRatioTest) {
  std::cout << "This test will see how your CMS insertion performance differs to one that is completely sequential."
            << std::endl;
  std::cout << "If your submission timeout, segfault, or you didn't implement the lock-free version, we will manually "
               "deduct all "
               "concurrent test points."
            << std::endl;

  std::vector<size_t> time_ms_with_mutex;
  std::vector<size_t> time_ms_wo_mutex;

  // Two Threads inserting 10000 entries each
  auto cms = CountMinSketch<int64_t>(500, 15);
  const int insert_iters = 10000;
  const int num_threads = 2;

  for (size_t iter = 0; iter < 10; iter++) {
    bool enable_mutex = iter % 2 == 0;
    std::vector<std::thread> threads;
    const int threads_count = num_threads;

    std::mutex g_mtx;
    auto start_time = std::chrono::system_clock::now();
    // Create threads that concurrently insert items
    for (size_t i = 0; i < threads_count; i++) {
      threads.emplace_back([&cms, &g_mtx, enable_mutex]() {
        for (int j = 0; j < insert_iters; j++) {
          if (enable_mutex) {
            // Test sequential execution
            auto latch = std::unique_lock<std::mutex>(g_mtx);
            cms.Insert(j % 10);
          } else {
            // Test lock-free execution
            cms.Insert(j % 10);
          }
        }
      });
    }
    // Wait for all threads to complete
    for (auto &thread : threads) {
      thread.join();
    }
    auto end_time = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    if (enable_mutex) {
      time_ms_with_mutex.push_back(time);
    } else {
      time_ms_wo_mutex.push_back(time);
    }
  }

  // Check result correctness
  for (int i = 0; i < 10; i++) {
    ASSERT_EQ(cms.Count(i), 20000);
  }

  std::cout << "<<< BEGIN" << std::endl;
  std::cout << "Multithreaded Insertion Time: ";
  double sum_1 = 0;
  double sum_2 = 0;
  for (auto x : time_ms_wo_mutex) {
    std::cout << x << " ";
    sum_1 += x;
  }
  std::cout << std::endl;

  std::cout << "Serialized Insertion Time: ";
  for (auto x : time_ms_with_mutex) {
    std::cout << x << " ";
    sum_2 += x;
  }
  std::cout << std::endl;
  double speedup = sum_2 / sum_1;
  std::cout << "Speedup: " << speedup << std::endl;
  std::cout << ">>> END" << std::endl;

  ASSERT_TRUE(speedup > 1.2);
}

}  // namespace bustub
