//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// disk_manager_test.cpp
//
// Identification: test/storage/disk_manager_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstring>

#include "common/config.h"
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

TEST_F(DiskManagerTest, DeletePageTest) {
  char buf[BUSTUB_PAGE_SIZE] = {0};
  char data[BUSTUB_PAGE_SIZE] = {0};
  auto dm = DiskManager(db_fname);
  auto initial_size = dm.GetDbFileSize();

  dm.ReadPage(0, buf);  // tolerate empty read

  std::strncpy(data, "A test string.", sizeof(data));
  size_t pages_to_write = 100;
  for (page_id_t page_id = 0; page_id < static_cast<page_id_t>(pages_to_write); page_id++) {
    dm.WritePage(page_id, data);
    dm.ReadPage(page_id, buf);
    EXPECT_EQ(std::memcmp(buf, data, sizeof(buf)), 0);
  }

  auto size_after_write = dm.GetDbFileSize();
  EXPECT_GE(size_after_write, initial_size);

  pages_to_write *= 2;
  std::strncpy(data, "test string version 2", sizeof(data));
  for (page_id_t page_id = 0; page_id < static_cast<page_id_t>(pages_to_write); page_id++) {
    dm.WritePage(page_id, data);
    dm.ReadPage(page_id, buf);
    EXPECT_EQ(std::memcmp(buf, data, sizeof(buf)), 0);

    dm.DeletePage(page_id);
  }

  // expect no change in file size after delete because we're reclaiming space
  auto size_after_delete = dm.GetDbFileSize();
  EXPECT_EQ(size_after_delete, size_after_write);

  dm.ShutDown();
}

// NOLINTNEXTLINE
TEST_F(DiskManagerTest, ThrowBadFileTest) {
  EXPECT_THROW(DiskManager("dev/null\\/foo/bar/baz/test.bustub"), Exception);
}

}  // namespace bustub
