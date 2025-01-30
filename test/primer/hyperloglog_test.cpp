//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hyperloglog_test.cpp
//
// Identification: test/primer/hyperloglog_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <limits>
#include <memory>
#include <string>
#include <thread>  //NOLINT
#include <vector>

#include "common/exception.h"
#include "gtest/gtest.h"
#include "primer/hyperloglog.h"
#include "primer/hyperloglog_presto.h"

namespace bustub {

TEST(HyperLogLogTest, DISABLED_BasicTest1) {
  auto obj = HyperLogLog<std::string>(static_cast<int16_t>(1));
  ASSERT_EQ(obj.GetCardinality(), 0);
  obj.AddElem("Welcome to CMU DB (15-445/645)");

  obj.ComputeCardinality();

  auto ans = obj.GetCardinality();
  ASSERT_EQ(ans, 2);

  for (uint64_t i = 0; i < 10; i++) {
    obj.AddElem("Andy");
    obj.AddElem("Connor");
    obj.AddElem("J-How");
    obj.AddElem("Kunle");
    obj.AddElem("Lan");
    obj.AddElem("Prashanth");
    obj.AddElem("William");
    obj.AddElem("Yash");
    obj.AddElem("Yuanxin");
    if (i == 0) {
      obj.ComputeCardinality();
      ans = obj.GetCardinality();
      ASSERT_EQ(ans, 6);
    }
  }

  obj.ComputeCardinality();
  ans = obj.GetCardinality();
  ASSERT_EQ(ans, 6);
}

TEST(HyperLogLogTest, DISABLED_BasicTest2) {
  auto obj = HyperLogLog<int64_t>(static_cast<int16_t>(3));

  ASSERT_EQ(obj.GetCardinality(), 0);

  obj.AddElem(0);

  obj.ComputeCardinality();
  auto ans = obj.GetCardinality();

  ASSERT_EQ(ans, 7);

  for (uint64_t i = 0; i < 10; i++) {
    obj.AddElem(10);
    obj.AddElem(122);
    obj.AddElem(200);
    obj.AddElem(911);
    obj.AddElem(999);
    obj.AddElem(1402);
    obj.AddElem(15445);
    obj.AddElem(15645);
    obj.AddElem(123456);
    obj.AddElem(312457);
    if (i == 0) {
      obj.ComputeCardinality();
      ans = obj.GetCardinality();
      ASSERT_EQ(ans, 10);
    }
  }

  for (uint64_t i = 0; i < 10; i++) {
    obj.AddElem(-1);
    obj.AddElem(-2);
    obj.AddElem(-3);
    obj.AddElem(-4);
    obj.AddElem(-5);
    obj.AddElem(-6);
    obj.AddElem(-7);
    obj.AddElem(-8);
    obj.AddElem(-9);
    obj.AddElem(-27);
    if (i == 0) {
      obj.ComputeCardinality();
      ans = obj.GetCardinality();
      ASSERT_EQ(ans, 10);
    }
  }
  obj.ComputeCardinality();
  ans = obj.GetCardinality();
  ASSERT_EQ(ans, 10);
}

TEST(HyperLogLogTest, DISABLED_EdgeTest1) {
  auto obj1 = HyperLogLog<int64_t>(static_cast<int16_t>(-2));
  obj1.ComputeCardinality();
  ASSERT_EQ(obj1.GetCardinality(), 0);
}

TEST(HyperLogLogTest, DISABLED_EdgeTest2) {
  auto obj = HyperLogLog<int64_t>(static_cast<int16_t>(0));
  obj.ComputeCardinality();
  ASSERT_EQ(obj.GetCardinality(), 0);

  obj.AddElem(1);
  obj.ComputeCardinality();
  ASSERT_EQ(obj.GetCardinality(), 1665180);

  obj.AddElem(-1);
  obj.ComputeCardinality();
  ASSERT_EQ(obj.GetCardinality(), 1665180);
}

TEST(HyperLogLogTest, DISABLED_BasicParallelTest) {
  auto obj = HyperLogLog<std::string>(static_cast<int16_t>(1));

  std::vector<std::thread> threads1;
  for (uint16_t i = 0; i < 10; i++) {
    threads1.emplace_back(std::thread([&]() { obj.AddElem("Welcome to CMU DB (15-445/645)"); }));
  }

  // Wait for all threads to finish execution
  for (auto &thread : threads1) {
    thread.join();
  }
  obj.ComputeCardinality();
  auto ans = obj.GetCardinality();
  ASSERT_EQ(ans, 2);

  std::vector<std::thread> threads2;
  for (uint16_t k = 0; k < 10; k++) {
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Andy"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Connor"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("J-How"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Kunle"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Lan"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Prashanth"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("William"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Yash"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Yuanxin"); }));
  }

  for (auto &thread : threads2) {
    thread.join();
  }

  obj.ComputeCardinality();
  ans = obj.GetCardinality();
  ASSERT_EQ(ans, 6);
}

TEST(HyperLogLogTest, DISABLED_ParallelTest1) {
  auto obj = HyperLogLog<std::string>(static_cast<int16_t>(14));

  std::vector<std::thread> threads1;
  for (uint16_t i = 0; i < 10; i++) {
    threads1.emplace_back(std::thread([&]() { obj.AddElem("Welcome to CMU DB (15-445/645)"); }));
  }

  // Wait for all threads to finish execution
  for (auto &thread : threads1) {
    thread.join();
  }
  obj.ComputeCardinality();
  auto ans = obj.GetCardinality();
  ASSERT_EQ(ans, 13009);

  std::vector<std::thread> threads2;
  for (uint64_t k = 0; k < 3000; k++) {
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Andy"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Connor"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("J-How"); }));
    threads2.emplace_back(std::thread([&]() { obj.AddElem("Kunle"); }));
  }

  for (auto &thread : threads2) {
    thread.join();
  }

  obj.ComputeCardinality();
  ans = obj.GetCardinality();
  ASSERT_EQ(ans, 13010);
}

TEST(HyperLogLogTest, DISABLED_PrestoBasicTest1) {
  auto obj = HyperLogLogPresto<std::string>(static_cast<int16_t>(2));
  ASSERT_EQ(obj.GetCardinality(), 0);

  std::string str1 = "Welcome to CMU DB (15-445/645)";

  obj.AddElem(str1);

  obj.ComputeCardinality();
  auto ans = obj.GetCardinality();
  ASSERT_EQ(ans, 3);
  for (uint64_t i = 0; i < 10; i++) {
    obj.AddElem("Andy");
    obj.AddElem("Connor");
    obj.AddElem("J-How");
    obj.AddElem("Kunle");
    obj.AddElem("Lan");
    obj.AddElem("Prashanth");
    obj.AddElem("William");
    obj.AddElem("Yash");
    obj.AddElem("Yuanxin");
    if (i == 0) {
      obj.ComputeCardinality();
      ans = obj.GetCardinality();
      ASSERT_EQ(ans, 4);
    }
  }

  obj.ComputeCardinality();
  ans = obj.GetCardinality();
  ASSERT_EQ(ans, 4);
}

TEST(HyperLogLogTest, DISABLED_PrestoCase1) {
  auto obj = HyperLogLogPresto<int64_t>(static_cast<int16_t>(1));
  auto ans = obj.GetCardinality();

  ASSERT_EQ(ans, 0);

  obj.AddElem(262144);

  obj.ComputeCardinality();
  ans = obj.GetCardinality();

  ASSERT_EQ(ans, 3);

  auto expected1 = obj.GetDenseBucket();
  ASSERT_EQ(2ULL, expected1[0].to_ullong());
  ASSERT_EQ(1, obj.GetOverflowBucketofIndex(0).to_ullong());

  obj.AddElem(0);

  obj.ComputeCardinality();
  ans = obj.GetCardinality();

  ASSERT_EQ(ans, 3);

  auto expected2 = obj.GetDenseBucket();
  ASSERT_EQ(15UL, expected2[0].to_ulong());
  ASSERT_EQ(3, obj.GetOverflowBucketofIndex(0).to_ullong());

  obj.AddElem(-9151314442816847872L);
  obj.ComputeCardinality();
  ans = obj.GetCardinality();

  ASSERT_EQ(ans, 227086569448168320UL);

  auto expected3 = obj.GetDenseBucket();
  ASSERT_EQ(8, expected3[1].to_ullong());
  ASSERT_EQ(3, obj.GetOverflowBucketofIndex(0).to_ullong());

  obj.AddElem(-1);

  obj.ComputeCardinality();
  ans = obj.GetCardinality();

  ASSERT_EQ(ans, 227086569448168320);
  auto expected4 = obj.GetDenseBucket();
  ASSERT_EQ(8, expected4[1].to_ullong());

  obj.AddElem(INT64_MIN);
  obj.ComputeCardinality();
  ans = obj.GetCardinality();

  ASSERT_EQ(ans, 14647083729406857216UL);
  auto expected5 = obj.GetDenseBucket();
  ASSERT_EQ(15UL, expected5[1].to_ulong());
}

TEST(HyperLogLogTest, DISABLED_PrestoCase2) {
  auto obj = HyperLogLogPresto<int64_t>(static_cast<int16_t>(0));
  auto ans = obj.GetCardinality();

  ASSERT_EQ(ans, 0);

  obj.AddElem(65536UL);
  ASSERT_EQ(obj.GetDenseBucket()[0].to_ullong(), 0);
  ASSERT_EQ(obj.GetOverflowBucketofIndex(0).to_ullong(), 1);

  obj.AddElem(INT64_MIN);
  obj.ComputeCardinality();
  ASSERT_EQ(obj.GetDenseBucket()[0].to_ullong(), 15);
  ASSERT_EQ(obj.GetOverflowBucketofIndex(0).to_ullong(), 3);

  obj.AddElem(0);
  obj.ComputeCardinality();
  ASSERT_EQ(obj.GetCardinality(), 14647083729406857216UL);
  ASSERT_EQ(obj.GetDenseBucket()[0].to_ullong(), 0);
  ASSERT_EQ(obj.GetOverflowBucketofIndex(0).to_ullong(), 4);
}

TEST(HyperLogLogTest, DISABLED_PrestoEdgeCase) {
  auto obj = HyperLogLogPresto<int64_t>(static_cast<int16_t>(-2));
  obj.ComputeCardinality();
  auto ans = obj.GetCardinality();

  ASSERT_EQ(ans, 0);
}

}  // namespace bustub
