//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager_test.cpp
//
// Identification: test/storage/disk/disk_manager_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstring>

#include "common/exception.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(DiskManagerTest, ReadWritePageTest) {
  remove("test.db");
  char buf[PAGE_SIZE] = {0};
  char data[PAGE_SIZE] = {0};
  std::string db_file("test.db");
  auto dm = DiskManager(db_file);
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
  remove(db_file.c_str());
}

// NOLINTNEXTLINE
TEST(DiskManagerTest, ReadWriteLogTest) {
  remove("test.db");
  remove("test.log");
  char buf[16] = {0};
  char data[16] = {0};
  std::string db_file("test.db");
  auto dm = DiskManager(db_file);
  std::strncpy(data, "A test string.", sizeof(data));

  dm.ReadLog(buf, sizeof(buf), 0);  // tolerate empty read

  dm.WriteLog(data, sizeof(data));
  dm.ReadLog(buf, sizeof(buf), 0);
  EXPECT_EQ(std::memcmp(buf, data, sizeof(buf)), 0);

  dm.ShutDown();
  remove(db_file.c_str());
  remove("test.log");
}

// NOLINTNEXTLINE
TEST(DiskManagerTest, ThrowBadFileTest) { EXPECT_THROW(DiskManager("dev/null\\/foo/bar/baz/test.db"), Exception); }

}  // namespace bustub
