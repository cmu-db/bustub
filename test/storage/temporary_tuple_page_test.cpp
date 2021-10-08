//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// temporary_tuple_page_test.cpp
//
// Identification: test/storage/temporary_tuple_page_test.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/temporary_tuple_page.h"

#include <iostream>
#include <memory>
#include <vector>

#include "buffer/buffer_pool_manager_instance.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager.h"
#include "type/value_factory.h"

/** For raw memory */
using byte_t = char;

namespace bustub {

TEST(TemporaryTuplePageTest, InitializationTest) {
  // Get some raw memory to work with
  auto page = std::make_unique<byte_t[]>(PAGE_SIZE);

  // Initialize the page
  auto *tmp = reinterpret_cast<TemporaryTuplePage *>(page.get());
  tmp->Init(0, PAGE_SIZE);
  EXPECT_EQ(PAGE_SIZE, tmp->GetPageSize());
  EXPECT_EQ(0, tmp->GetTupleCount());
}

TEST(TemporaryTuplePageTest, InsertTest0) {
  // Get some raw memory to work with
  auto page = std::make_unique<byte_t[]>(PAGE_SIZE);

  // Initialize the page
  auto *tmp = reinterpret_cast<TemporaryTuplePage *>(page.get());
  tmp->Init(0, PAGE_SIZE);
  EXPECT_EQ(0, tmp->GetTupleCount());

  // Create a tuple
  const std::vector<Column> columns{{"a", TypeId::INTEGER}, {"b", TypeId::INTEGER}};
  const Schema schema{columns};
  const std::vector<Value> values{ValueFactory::GetIntegerValue(0), ValueFactory::GetIntegerValue(1)};
  Tuple tuple{values, &schema};

  // Page should have sufficient space
  EXPECT_TRUE(tmp->HasSufficientSpaceFor(tuple));
  tmp->Insert(tuple);

  // Insert should succeed
  EXPECT_EQ(1, tmp->GetTupleCount());
}

TEST(TemporaryTuplePageTest, InsertTest1) {
  // Get some raw memory to work with
  auto page = std::make_unique<byte_t[]>(PAGE_SIZE);

  // Initialize the page
  auto *tmp = reinterpret_cast<TemporaryTuplePage *>(page.get());
  tmp->Init(0, PAGE_SIZE);
  EXPECT_EQ(0, tmp->GetTupleCount());

  // Create a tuple
  const std::vector<Column> columns{{"a", TypeId::INTEGER}, {"b", TypeId::INTEGER}, {"c", TypeId::INTEGER}};
  const Schema schema{columns};
  const std::vector<Value> values{ValueFactory::GetIntegerValue(0), ValueFactory::GetIntegerValue(1),
                                  ValueFactory::GetIntegerValue(2)};
  Tuple tuple{values, &schema};

  // The size of this tuple is 12 bytes; the total size of
  // a page is 4096 bytes; we reserve the first 20 bytes
  // for the page prefix content, and each tuple is prefixed
  // by a 4-byte size field, for a total of 16 bytes per tuple;
  // this implies we should have space for (4096 - 20) / 16 = 254
  // tuples in the page before it is exhausted

  // Insert the maximum number of tuples
  for (std::size_t i = 0; i < 254; ++i) {
    EXPECT_TRUE(tmp->HasSufficientSpaceFor(tuple));
    tmp->Insert(tuple);
    EXPECT_EQ(i + 1, tmp->GetTupleCount());
    EXPECT_EQ(PAGE_SIZE, tmp->GetPageSize());
  }

  // The page should no longer have space for a tuple
  EXPECT_FALSE(tmp->HasSufficientSpaceFor(tuple));
}

TEST(TemporaryTuplePageTest, InsertTest2) {
  // Get some raw memory to work with
  auto page = std::make_unique<byte_t[]>(PAGE_SIZE);

  // Initialize the page
  auto *tmp = reinterpret_cast<TemporaryTuplePage *>(page.get());
  tmp->Init(0, PAGE_SIZE);
  EXPECT_EQ(0, tmp->GetTupleCount());

  // Create a tuple
  const std::vector<Column> columns{{"a", TypeId::INTEGER}, {"b", TypeId::INTEGER}, {"c", TypeId::INTEGER}};
  const Schema schema{columns};
  const std::vector<Value> values{ValueFactory::GetIntegerValue(0), ValueFactory::GetIntegerValue(1),
                                  ValueFactory::GetIntegerValue(2)};
  Tuple tuple{values, &schema};

  // Insert the tuple
  EXPECT_TRUE(tmp->HasSufficientSpaceFor(tuple));
  tmp->Insert(tuple);

  // Attempt to read the tuple back
  std::size_t count = 0;
  for (auto it = tmp->Begin(); it != tmp->End(); ++it) {
    Tuple read = *it;
    EXPECT_EQ(0, read.GetValue(&schema, 0).GetAs<int32_t>());
    EXPECT_EQ(1, read.GetValue(&schema, 1).GetAs<int32_t>());
    EXPECT_EQ(2, read.GetValue(&schema, 2).GetAs<int32_t>());
    ++count;
  }

  // We should only read a single tuple
  EXPECT_EQ(1, count);
}

TEST(TemporaryTuplePageTest, InsertTest3) {
  // Get some raw memory to work with
  auto page = std::make_unique<byte_t[]>(PAGE_SIZE);

  // Initialize the page
  auto *tmp = reinterpret_cast<TemporaryTuplePage *>(page.get());
  tmp->Init(0, PAGE_SIZE);
  EXPECT_EQ(0, tmp->GetTupleCount());

  // Create a tuple
  const std::vector<Column> columns{{"a", TypeId::INTEGER}, {"b", TypeId::INTEGER}, {"c", TypeId::INTEGER}};
  const Schema schema{columns};

  // Insert the maximum number of tuples
  for (std::size_t i = 0; i < 254; ++i) {
    const std::vector<Value> values{ValueFactory::GetIntegerValue(i), ValueFactory::GetIntegerValue(i + 1),
                                    ValueFactory::GetIntegerValue(i + 2)};
    Tuple tuple{values, &schema};
    EXPECT_TRUE(tmp->HasSufficientSpaceFor(tuple));
    tmp->Insert(tuple);
  }

  // Attempt to read all tuples back

  // Attempt to read the tuple back
  std::vector<Tuple> read_tuples{};
  for (auto it = tmp->Begin(); it != tmp->End(); ++it) {
    // Iteration proceeds in reverse order of insertion
    read_tuples.push_back(*it);
  }

  // We should have read all the tuples
  EXPECT_EQ(254, read_tuples.size());

  // Tuples are read in reverse order of insertion
  std::reverse(read_tuples.begin(), read_tuples.end());
  for (std::size_t i = 0; i < 254; ++i) {
    const auto &read = read_tuples[i];
    EXPECT_EQ(static_cast<int32_t>(i), read.GetValue(&schema, 0).GetAs<int32_t>());
    EXPECT_EQ(static_cast<int32_t>(i + 1), read.GetValue(&schema, 1).GetAs<int32_t>());
    EXPECT_EQ(static_cast<int32_t>(i + 2), read.GetValue(&schema, 2).GetAs<int32_t>());
  }
}

}  // namespace bustub
