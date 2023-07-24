//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// page_guard_test.cpp
//
// Identification: test/storage/page_guard_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>
#include <memory>
#include <random>
#include <string>

#include "buffer/buffer_pool_manager.h"
#include "common/config.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/page/page_guard.h"

#include "gtest/gtest.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(PageGuardTest, DISABLED_GuardUpgradeBasicTest1) {
  const size_t buffer_pool_size = 5;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_shared<BufferPoolManager>(buffer_pool_size, disk_manager.get(), k);

  // Create two dummy pages from BPM
  page_id_t p_1 = -1;
  page_id_t p_2 = -1;
  Page *dummy_page_1 = bpm->NewPage(&p_1);
  Page *dummy_page_2 = bpm->NewPage(&p_2);
  // Now the pin counts should both be 1
  EXPECT_EQ(dummy_page_1->GetPinCount(), 1);
  EXPECT_EQ(dummy_page_2->GetPinCount(), 1);
  EXPECT_NE(p_1, -1);
  EXPECT_NE(p_2, -1);
  EXPECT_NE(dummy_page_1, nullptr);
  EXPECT_NE(dummy_page_2, nullptr);

  // Unpin the two pages
  bpm->UnpinPage(p_1, false);
  bpm->UnpinPage(p_2, false);

  // Create two dummy BasicPageGuards
  auto b_pg_1 = bpm->FetchPageBasic(p_1);
  auto b_pg_2 = bpm->FetchPageBasic(p_2);

  // Upgrade the first PageGuard to ReadPageGuard
  ReadPageGuard rpg = b_pg_1.UpgradeRead();

  // Sanity Check
  EXPECT_EQ(dummy_page_1->GetPinCount(), 1);
  EXPECT_EQ(b_pg_1.GetPage(), nullptr);
  EXPECT_EQ(b_pg_1.GetBPM(), nullptr);
  EXPECT_EQ(rpg.GetPage(), dummy_page_1);
  EXPECT_EQ(rpg.GetBPM(), bpm.get());

  // Drop the first ReadPageGuard
  rpg.Drop();
  EXPECT_EQ(dummy_page_1->GetPinCount(), 0);

  // Upgrade the second PageGuard to WritePageGuard
  WritePageGuard wpg = b_pg_2.UpgradeWrite();

  // Sanity Check
  EXPECT_EQ(dummy_page_2->GetPinCount(), 1);
  EXPECT_EQ(b_pg_2.GetPage(), nullptr);
  EXPECT_EQ(b_pg_2.GetBPM(), nullptr);
  EXPECT_EQ(wpg.GetPage(), dummy_page_2);
  EXPECT_EQ(wpg.GetBPM(), bpm.get());

  // Drop the second WritePageGuard
  wpg.Drop();
  EXPECT_EQ(dummy_page_2->GetPinCount(), 0);

  // Shut down the DiskManager
  disk_manager->ShutDown();
}

// NOLINTNEXTLINE
TEST(PageGuardTest, DISABLED_GuardUpgradeBasicTest2) {
  const size_t buffer_pool_size = 5;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_shared<BufferPoolManager>(buffer_pool_size, disk_manager.get(), k);

  // Create one dummy pages
  page_id_t p_id = -1;
  Page *dummy_page = bpm->NewPage(&p_id);
  // The pin count should be 1
  EXPECT_EQ(dummy_page->GetPinCount(), 1);
  EXPECT_NE(p_id, -1);
  EXPECT_NE(dummy_page, nullptr);

  // Unpin the page
  bpm->UnpinPage(p_id, false);

  // Create one dummy BasicPageGuard
  auto bpg = bpm->FetchPageBasic(p_id);

  // Upgrade the PageGuard to ReadPageGuard
  ReadPageGuard rpg = bpg.UpgradeRead();

  // Sanity Check
  EXPECT_EQ(dummy_page->GetPinCount(), 1);
  EXPECT_EQ(bpg.GetPage(), nullptr);
  EXPECT_EQ(bpg.GetBPM(), nullptr);
  EXPECT_EQ(rpg.GetPage(), dummy_page);
  EXPECT_EQ(rpg.GetBPM(), bpm.get());

  // Upgrade the rpg to WritePageGuard
  WritePageGuard wpg = rpg.UpgradeWrite();

  // Sanity Check
  EXPECT_EQ(dummy_page->GetPinCount(), 1);
  EXPECT_EQ(rpg.GetPage(), nullptr);
  EXPECT_EQ(rpg.GetBPM(), nullptr);
  EXPECT_EQ(wpg.GetPage(), dummy_page);
  EXPECT_EQ(wpg.GetBPM(), bpm.get());

  // Drop the WritePageGuard
  wpg.Drop();
  EXPECT_EQ(dummy_page->GetPinCount(), 0);

  // Shut down the DiskManager
  disk_manager->ShutDown();
}

// NOLINTNEXTLINE
TEST(PageGuardTest, DISABLED_GuardUpgradeConcurrentTest1) {
  // This test should not block
  const size_t buffer_pool_size = 20;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_shared<BufferPoolManager>(buffer_pool_size, disk_manager.get(), k);

  // Create ten dummy pages, construct a page group
  std::vector<page_id_t> p_g;
  std::vector<Page *> p_vec;

  // Reserve space for p_g, since clang-tidy complains
  p_g.reserve(10);

  // Initialize the page group
  for (int i = 0; i < 10; ++i) {
    page_id_t tmp_pid = -1;
    Page *tmp_page = bpm->NewPage(&tmp_pid);
    EXPECT_EQ(tmp_page->GetPinCount(), 1);
    EXPECT_NE(tmp_pid, -1);
    // Unpin the page
    bpm->UnpinPage(tmp_pid, false);
    p_g.push_back(tmp_pid);
    p_vec.push_back(tmp_page);
  }

  // Create a bpg group
  std::vector<BasicPageGuard> bpg_g;

  // Create a rpg group
  std::vector<std::pair<ReadPageGuard, int>> rpg_g;

  // Reserve space for bpg_g & rpg_g, same as above
  bpg_g.reserve(10);
  rpg_g.reserve(10);

  // Initialize bpg_g
  for (int i = 0; i < 10; ++i) {
    bpg_g.push_back(bpm->FetchPageBasic(p_g[i]));
    EXPECT_EQ(bpg_g.back().GetPage()->GetPinCount(), 1);
  }

  // Create the thread vector
  std::vector<std::thread> t_g;

  // Reserve space
  t_g.reserve(10);

  // Launch ten threads, performing the upgrade
  for (int i = 0; i < 10; ++i) {
    // Capture i by value here, to avoid potential race condition
    t_g.emplace_back([i, &bpg_g, &rpg_g]() { rpg_g.emplace_back(bpg_g[i].UpgradeRead(), i); });
  }

  // Wait til all the upgrades are finished
  for (auto &t : t_g) {
    t.join();
  }

  // Sanity check
  for (auto &[rpg, index] : rpg_g) {
    EXPECT_EQ(bpg_g[index].GetPage(), nullptr);
    EXPECT_EQ(bpg_g[index].GetBPM(), nullptr);
    EXPECT_EQ(rpg.GetPage(), p_vec[index]);
    EXPECT_EQ(rpg.GetBPM(), bpm.get());
    // The pin count should be 1 for each page
    EXPECT_EQ(rpg.GetPage()->GetPinCount(), 1);
    // Drop the ReadPageGuard
    rpg.Drop();
    EXPECT_EQ(p_vec[index]->GetPinCount(), 0);
  }

  // Shutdown the Disk Manager
  disk_manager->ShutDown();
}

TEST(PageGuardTest, DISABLED_GuardUpgradeConcurrentTest2) {
  const size_t buffer_pool_size = 5;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_shared<BufferPoolManager>(buffer_pool_size, disk_manager.get(), k);

  // Create one dummy page
  page_id_t p_id = -1;
  Page *dummy_page = bpm->NewPage(&p_id);

  // Unpin the page
  bpm->UnpinPage(p_id, false);

  // Fetch a ReadPageGuard
  ReadPageGuard rpg_1 = bpm->FetchPageRead(p_id);

  // Fetch a second time, assumes this is from another thread
  ReadPageGuard rpg_2 = bpm->FetchPageRead(p_id);

  // Current pin count should be 1
  EXPECT_EQ(dummy_page->GetPinCount(), 2);

  // Update the first ReadPageGuard to WritePageGuard in a background thread
  // Which should block at present
  std::thread t([&]() {
    WritePageGuard wpg = rpg_1.UpgradeWrite();
    EXPECT_EQ(wpg.GetPage(), dummy_page);
    EXPECT_EQ(dummy_page->GetPinCount(), 1);

    // Drop the WritePageGuard
    wpg.Drop();
    EXPECT_EQ(dummy_page->GetPinCount(), 0);
  });

  // Drop the second ReadPageGuard
  rpg_2.Drop();
  EXPECT_EQ(rpg_2.GetPage(), nullptr);

  // This should not block
  t.join();

  // Sanity check
  EXPECT_EQ(dummy_page->GetPinCount(), 0);
}

// NOLINTNEXTLINE
TEST(PageGuardTest, DISABLED_SampleTest) {
  const size_t buffer_pool_size = 5;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_shared<BufferPoolManager>(buffer_pool_size, disk_manager.get(), k);

  page_id_t page_id_temp;
  auto *page0 = bpm->NewPage(&page_id_temp);

  auto guarded_page = BasicPageGuard{bpm.get(), page0};

  EXPECT_EQ(page0->GetData(), guarded_page.GetData());
  EXPECT_EQ(page0->GetPageId(), guarded_page.PageId());
  EXPECT_EQ(1, page0->GetPinCount());

  guarded_page.Drop();

  EXPECT_EQ(0, page0->GetPinCount());

  // Shutdown the disk manager and remove the temporary file we created.
  disk_manager->ShutDown();
}

}  // namespace bustub
