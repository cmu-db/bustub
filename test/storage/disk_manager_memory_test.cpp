//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager_memory_test.cpp
//
// Identification: test/storage/disk/disk_manager_memory_test.cpp
//
// Copyright (c) 2015-2020, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstring>

#include "common/exception.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager_memory.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(DiskManagerMemoryTest, ReadWritePageTest) {
  char buf[PAGE_SIZE] = {0};
  char data[PAGE_SIZE] = {0};
  auto dm = DiskManagerMemory();
  std::strncpy(data, "A test string.", sizeof(data));

  dm.ReadPage(0, buf);  // tolerate empty read

  dm.WritePage(0, data);
  dm.ReadPage(0, buf);
  EXPECT_EQ(std::memcmp(buf, data, sizeof(buf)), 0);

  std::memset(buf, 0, sizeof(buf));
  dm.WritePage(5, data);
  dm.ReadPage(5, buf);
  EXPECT_EQ(std::memcmp(buf, data, sizeof(buf)), 0);

  dm.ShutDown();
}

}  // namespace bustub
