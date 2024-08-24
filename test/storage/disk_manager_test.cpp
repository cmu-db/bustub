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

static std::filesystem::path db_fname("test.bustub");
static std::filesystem::path log_fname("test.log");

class DiskManagerTest : public ::testing::Test {
 protected:
  // This function is called before every test.
  void SetUp() override {
    remove(db_fname);
    remove(log_fname);
  }

  // This function is called after every test.
  void TearDown() override {
    remove(db_fname);
    remove(log_fname);
  };
};

// NOLINTNEXTLINE
TEST_F(DiskManagerTest, ReadWritePageTest) {
  char buf[BUSTUB_PAGE_SIZE] = {0};
  char data[BUSTUB_PAGE_SIZE] = {0};
  auto dm = DiskManager(db_fname);
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

// NOLINTNEXTLINE
TEST_F(DiskManagerTest, ReadWriteLogTest) {
  char buf[16] = {0};
  char data[16] = {0};
  auto dm = DiskManager(db_fname);
  std::strncpy(data, "A test string.", sizeof(data));

  dm.ReadLog(buf, sizeof(buf), 0);  // tolerate empty read

  dm.WriteLog(data, sizeof(data));
  dm.ReadLog(buf, sizeof(buf), 0);
  EXPECT_EQ(std::memcmp(buf, data, sizeof(buf)), 0);

  dm.ShutDown();
}

// NOLINTNEXTLINE
TEST_F(DiskManagerTest, ThrowBadFileTest) {
  EXPECT_THROW(DiskManager("dev/null\\/foo/bar/baz/test.bustub"), Exception);
}

}  // namespace bustub
