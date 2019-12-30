//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// counter.h
//
// Identification: test/buffer/counter.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include "gtest/gtest.h"

namespace bustub {

enum FuncType { FetchPage, UnpinPage, FlushPage, NewPage, DeletePage, FlushAllPages };

struct Counter {
  // 0-FetchPage  1-UnpinPage  2-FlushPage 3-NewPage 4-DeletePage
  // 5-FlushAllPages
  static const int num_types = 6;
  std::atomic_int counts[num_types];

  void Reset() {
    for (auto &count : counts) {
      count = 0;
    }
  }

  void AddCount(FuncType func_type) { ++counts[func_type]; }

  // Make sure fetch page function only calls fetch page once and
  // does not call other functions
  void CheckFetchPage() {
    EXPECT_EQ(counts[0], 1) << "has to call FetchPage once";
    for (int i = 1; i < num_types; ++i) {
      EXPECT_EQ(counts[i], 0) << "FetchPage Should not call other functions";
    }
  }

  void CheckUnpinPage() {
    EXPECT_EQ(counts[1], 1) << "has to call UnpinPage once";
    for (int i = 0; i < num_types; ++i) {
      if (i != 1) {
        EXPECT_EQ(counts[i], 0) << "UnPinPage Should not call other functions";
      }
    }
  }

  void CheckFlushPage() {
    EXPECT_EQ(counts[2], 1) << "has to call FlushPage once";
    for (int i = 0; i < num_types; ++i) {
      if (i != 2) {
        EXPECT_EQ(counts[i], 0) << "FlushPage Should not call other functions";
      }
    }
  }

  void CheckNewPage() {
    EXPECT_EQ(counts[3], 1) << "has to call NewPage once";
    for (int i = 0; i < num_types; ++i) {
      if (i != 3) {
        EXPECT_EQ(counts[i], 0) << "NewPage Should not call other functions";
      }
    }
  }

  void CheckDeletePage() {
    EXPECT_EQ(counts[4], 1) << "has to call DeletePage once";
    for (int i = 0; i < num_types; ++i) {
      if (i != 4) {
        EXPECT_EQ(counts[i], 0) << "DeletePage Should not call other functions";
      }
    }
  }

  void CheckFlushAllPages() {
    for (int i = 1; i < 5; ++i) {
      EXPECT_EQ(counts[i], 0) << "FlushAllPage Should not call other functions";
    }
    EXPECT_EQ(counts[5], 1) << "has to call FlushAllPage once";
  }
};

}  // namespace bustub
