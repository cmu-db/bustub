//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager_test.cpp
//
// Identification: test/buffer/buffer_pool_manager_test.cpp
//
// Copyright (c) 2015-2024, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <deque>
#include <filesystem>

#include "buffer/buffer_pool_manager.h"
#include "gtest/gtest.h"
#include "storage/page/page_guard.h"

namespace bustub {

static std::filesystem::path db_fname("test.bustub");

// The number of frames we give to the buffer pool.
const size_t FRAMES = 10;
// Note that this test assumes you are using the an LRU-K replacement policy.
const size_t K_DIST = 5;

TEST(BufferPoolManagerTest, DISABLED_VeryBasicTest) {
  // A very basic test.

  auto disk_manager = std::make_shared<DiskManager>(db_fname);
  auto bpm = std::make_shared<BufferPoolManager>(FRAMES, disk_manager.get(), K_DIST);

  page_id_t pid = bpm->NewPage();

  char str[] = "Hello, world!";

  // Check `WritePageGuard` basic functionality.
  {
    auto guard = bpm->WritePage(pid);
    char *data = guard.GetDataMut();
    snprintf(data, sizeof(str), "%s", str);
    EXPECT_STREQ(data, str);
  }

  // Check `ReadPageGuard` basic functionality.
  {
    auto guard = bpm->ReadPage(pid);
    const char *data = guard.GetData();
    EXPECT_STREQ(data, str);
  }

  // Check `ReadPageGuard` basic functionality (again).
  {
    auto guard = bpm->ReadPage(pid);
    const char *data = guard.GetData();
    EXPECT_STREQ(data, str);
  }

  ASSERT_TRUE(bpm->DeletePage(pid));
}

TEST(BufferPoolManagerTest, DISABLED_PagePinEasyTest) {
  auto disk_manager = std::make_shared<DiskManager>(db_fname);
  auto bpm = std::make_shared<BufferPoolManager>(2, disk_manager.get(), 5);

  page_id_t pageid0;
  page_id_t pageid1;

  {
    pageid0 = bpm->NewPage();
    auto page0_write_opt = bpm->CheckedWritePage(pageid0);
    ASSERT_TRUE(page0_write_opt.has_value());
    WritePageGuard page0_write = std::move(page0_write_opt.value());
    strcpy(page0_write.GetDataMut(), "page0");  // NOLINT

    pageid1 = bpm->NewPage();
    auto page1_write_opt = bpm->CheckedWritePage(pageid1);
    ASSERT_TRUE(page1_write_opt.has_value());
    WritePageGuard page1_write = std::move(page1_write_opt.value());
    strcpy(page1_write.GetDataMut(), "page1");  // NOLINT

    ASSERT_EQ(1, bpm->GetPinCount(pageid0));
    ASSERT_EQ(1, bpm->GetPinCount(pageid1));

    page_id_t temp_page_id1 = bpm->NewPage();
    auto temp_page1_opt = bpm->CheckedReadPage(temp_page_id1);
    ASSERT_FALSE(temp_page1_opt.has_value());

    page_id_t temp_page_id2 = bpm->NewPage();
    auto temp_page2_opt = bpm->CheckedWritePage(temp_page_id2);
    ASSERT_FALSE(temp_page2_opt.has_value());

    ASSERT_EQ(1, bpm->GetPinCount(pageid0));
    page0_write.Drop();
    ASSERT_EQ(0, bpm->GetPinCount(pageid0));

    ASSERT_EQ(1, bpm->GetPinCount(pageid1));
    page1_write.Drop();
    ASSERT_EQ(0, bpm->GetPinCount(pageid0));
  }

  {
    page_id_t temp_page_id1 = bpm->NewPage();
    auto temp_page1_opt = bpm->CheckedReadPage(temp_page_id1);
    ASSERT_TRUE(temp_page1_opt.has_value());

    page_id_t temp_page_id2 = bpm->NewPage();
    auto temp_page2_opt = bpm->CheckedWritePage(temp_page_id2);
    ASSERT_TRUE(temp_page2_opt.has_value());

    ASSERT_FALSE(bpm->GetPinCount(pageid0).has_value());
    ASSERT_FALSE(bpm->GetPinCount(pageid1).has_value());
  }

  {
    auto page0_write_opt = bpm->CheckedWritePage(pageid0);
    ASSERT_TRUE(page0_write_opt.has_value());
    WritePageGuard page0_write = std::move(page0_write_opt.value());
    ASSERT_EQ(0, strcmp(page0_write.GetData(), "page0"));
    strcpy(page0_write.GetDataMut(), "page0updated");  // NOLINT

    auto page1_write_opt = bpm->CheckedWritePage(pageid1);
    ASSERT_TRUE(page1_write_opt.has_value());
    WritePageGuard page1_write = std::move(page1_write_opt.value());
    ASSERT_EQ(0, strcmp(page1_write.GetData(), "page1"));
    strcpy(page1_write.GetDataMut(), "page1updated");  // NOLINT

    ASSERT_EQ(1, bpm->GetPinCount(pageid0));
    ASSERT_EQ(1, bpm->GetPinCount(pageid1));
  }

  ASSERT_EQ(0, bpm->GetPinCount(pageid0));
  ASSERT_EQ(0, bpm->GetPinCount(pageid1));

  {
    auto page0_read_opt = bpm->CheckedReadPage(pageid0);
    ASSERT_TRUE(page0_read_opt.has_value());
    ReadPageGuard page0_read = std::move(page0_read_opt.value());
    ASSERT_EQ(0, strcmp(page0_read.GetData(), "page0updated"));

    auto page1_read_opt = bpm->CheckedReadPage(pageid1);
    ASSERT_TRUE(page1_read_opt.has_value());
    ReadPageGuard page1_read = std::move(page1_read_opt.value());
    ASSERT_EQ(0, strcmp(page1_read.GetData(), "page1updated"));

    ASSERT_EQ(1, bpm->GetPinCount(pageid0));
    ASSERT_EQ(1, bpm->GetPinCount(pageid1));
  }

  ASSERT_EQ(0, bpm->GetPinCount(pageid0));
  ASSERT_EQ(0, bpm->GetPinCount(pageid1));

  remove(db_fname);
  remove(disk_manager->GetLogFileName());
}

TEST(BufferPoolManagerTest, DISABLED_PagePinMediumTest) {
  auto disk_manager = std::make_shared<DiskManager>(db_fname);
  auto bpm = std::make_shared<BufferPoolManager>(FRAMES, disk_manager.get(), K_DIST);

  // Scenario: The buffer pool is empty. We should be able to create a new page.
  page_id_t pid0 = bpm->NewPage();
  auto page0 = bpm->WritePage(pid0);

  // Scenario: Once we have a page, we should be able to read and write content.
  snprintf(page0.GetDataMut(), BUSTUB_PAGE_SIZE, "Hello");
  EXPECT_EQ(0, strcmp(page0.GetDataMut(), "Hello"));

  // Create a vector of unique pointers to page guards, which prevents the guards from getting destructed.
  std::deque<WritePageGuard> pages;
  pages.push_back(std::move(page0));

  // Scenario: We should be able to create new pages until we fill up the buffer pool.
  for (size_t i = 1; i < FRAMES; i++) {
    auto pid = bpm->NewPage();
    auto page = bpm->WritePage(pid);
    pages.push_back(std::move(page));
  }

  // Scenario: All of the pin counts should be 1.
  for (const auto &page : pages) {
    auto pid = page.GetPageId();
    EXPECT_EQ(1, bpm->GetPinCount(pid));
  }

  // Scenario: Once the buffer pool is full, we should not be able to create any new pages.
  for (size_t i = 0; i < 10; i++) {
    auto pid = bpm->NewPage();
    auto fail = bpm->CheckedWritePage(pid);
    ASSERT_FALSE(fail.has_value());
  }

  // Scenario: Drop the first 5 pages to unpin them.
  for (size_t i = 0; i < 5; i++) {
    page_id_t pid = pages[0].GetPageId();
    EXPECT_EQ(1, bpm->GetPinCount(pid));
    pages.pop_front();
    EXPECT_EQ(0, bpm->GetPinCount(pid));
  }

  // Scenario: All of the pin counts of the pages we haven't dropped yet should still be 1.
  for (const auto &page : pages) {
    auto pid = page.GetPageId();
    EXPECT_EQ(1, bpm->GetPinCount(pid));
  }

  // Scenario: After unpinning pages {0, 1, 2, 3, 4}, we should be able to create 4 new pages and bring them into
  // memory. Bringing those 4 pages into memory should evict the first 4 pages {0, 1, 2, 3} because of LRU.
  for (size_t i = 0; i < 4; i++) {
    auto pid = bpm->NewPage();
    auto page = bpm->WritePage(pid);
    pages.push_back(std::move(page));
  }

  // Scenario: There should be one frame available, and we should be able to fetch the data we wrote a while ago.
  {
    ReadPageGuard original_page = bpm->ReadPage(pid0);
    EXPECT_EQ(0, strcmp(original_page.GetData(), "Hello"));
  }

  // Scenario: Once we unpin page 0 and then make a new page, all the buffer pages should now be pinned. Fetching page 0
  // again should fail.
  auto last_pid = bpm->NewPage();
  auto last_page = bpm->ReadPage(last_pid);

  auto fail = bpm->CheckedReadPage(pid0);
  ASSERT_FALSE(fail.has_value());

  // Shutdown the disk manager and remove the temporary file we created.
  disk_manager->ShutDown();
  remove(db_fname);
}

TEST(BufferPoolManagerTest, DISABLED_ContentionTest) {
  auto disk_manager = std::make_shared<DiskManager>(db_fname);
  auto bpm = std::make_shared<BufferPoolManager>(FRAMES, disk_manager.get(), K_DIST);

  const size_t rounds = 100000;

  auto pid = bpm->NewPage();

  auto thread1 = std::thread([&]() {
    for (size_t i = 0; i < rounds; i++) {
      auto guard = bpm->WritePage(pid);
      strcpy(guard.GetDataMut(), std::to_string(i).c_str());  // NOLINT
    }
  });

  auto thread2 = std::thread([&]() {
    for (size_t i = 0; i > rounds; i++) {
      auto guard = bpm->WritePage(pid);
      strcpy(guard.GetDataMut(), std::to_string(i).c_str());  // NOLINT
    }
  });

  auto thread3 = std::thread([&]() {
    for (size_t i = 0; i > rounds; i++) {
      auto guard = bpm->WritePage(pid);
      strcpy(guard.GetDataMut(), std::to_string(i).c_str());  // NOLINT
    }
  });

  auto thread4 = std::thread([&]() {
    for (size_t i = 0; i > rounds; i++) {
      auto guard = bpm->WritePage(pid);
      strcpy(guard.GetDataMut(), std::to_string(i).c_str());  // NOLINT
    }
  });

  thread3.join();
  thread2.join();
  thread4.join();
  thread1.join();
}

}  // namespace bustub
