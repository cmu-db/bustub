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
#include <thread>

#include "buffer/buffer_pool_manager.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/page/page_guard.h"

#include "gtest/gtest.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(PageGuardTest, DISABLED_GuardUpgradeBasicTest1) {
  const size_t buffer_pool_size = 5;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto *bpm = new BufferPoolManager(buffer_pool_size, disk_manager.get(), k);

  // Create two dummy pages
  Page *dummy_page_1 = new Page();
  Page *dummy_page_2 = new Page();

  // Create two dummy BasicPageGuards
  auto b_pg_1 = BasicPageGuard{bpm, dummy_page_1};
  auto b_pg_2 = BasicPageGuard{bpm, dummy_page_2};

  // Upgrade the first pg to ReadPageGuard
  ReadPageGuard rpg = b_pg_1.UpgradeRead();

  // Sanity Check
  assert(b_pg_1.GetPage() == nullptr && b_pg_1.GetBPM() == nullptr);
  assert(rpg.GetPage() == dummy_page_1 && rpg.GetBPM() == bpm);

  // Upgrade the second pg to WritePageGuard
  WritePageGuard wpg = b_pg_2.UpgradeWrite();

  // Sanity Check
  assert(b_pg_2.GetPage() == nullptr && b_pg_2.GetBPM() == nullptr);
  assert(wpg.GetPage() == dummy_page_2 && wpg.GetBPM() == bpm);

  // Clean the resource
  delete dummy_page_2;
  delete dummy_page_1;
  delete bpm;

  // Shut down the DiskManager
  disk_manager->ShutDown();
}

// NOLINTNEXTLINE
TEST(PageGuardTest, DISABLED_GuardUpgradeBasicTest2) {
  const size_t buffer_pool_size = 5;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto *bpm = new BufferPoolManager(buffer_pool_size, disk_manager.get(), k);

  // Create one dummy pages
  Page *dummy_page = new Page();

  // Create one dummy BasicPageGuard
  auto b_pg = BasicPageGuard{bpm, dummy_page};

  // Upgrade the pg to ReadPageGuard
  ReadPageGuard rpg = b_pg.UpgradeRead();

  // Sanity Check
  assert(b_pg.GetPage() == nullptr && b_pg.GetBPM() == nullptr);
  assert(rpg.GetPage() == dummy_page && rpg.GetBPM() == bpm);

  // Upgrade the rpg to WritePageGuard
  WritePageGuard wpg = rpg.UpgradeWrite();

  // Sanity Check
  assert(rpg.GetPage() == nullptr && rpg.GetBPM() == nullptr);
  assert(wpg.GetPage() == dummy_page && wpg.GetBPM() == bpm);

  // Clean the resource
  delete dummy_page;
  delete bpm;

  // Shut down the DiskManager
  disk_manager->ShutDown();
}

// NOLINTNEXTLINE
TEST(PageGuardTest, DISABLED_GuardUpgradeConcurrentTest1) {
  // This test should not block
  const size_t buffer_pool_size = 5;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto *bpm = new BufferPoolManager(buffer_pool_size, disk_manager.get(), k);

  // Create ten dummy pages, construct a page group
  std::vector<std::shared_ptr<Page>> p_g;

  // Reserve space for p_g, since clang-tidy complains
  p_g.reserve(10);

  // Initialize the page group
  for (int i = 0; i < 10; ++i) {
    p_g.push_back(std::make_shared<Page>());
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
    bpg_g.emplace_back(bpm, p_g[i].get());
  }

  // Create the thread vector
  std::vector<std::thread> t_g;

  // Reserve space
  t_g.reserve(10);

  // Launch ten threads, performing the upgrade
  for (int i = 0; i < 10; ++i) {
    // Capture i by value here, to avoid potential race condition
    t_g.emplace_back([i, &bpg_g, &rpg_g]() {
      rpg_g.emplace_back(bpg_g[i].UpgradeRead(), i);
    });
  }

  // Wait til all the upgrades are finished
  for (auto &t : t_g) {
    t.join();
  }

  // Sanity check
  for (const auto &[rpg, index] : rpg_g) {
    assert(bpg_g[index].GetPage() == nullptr && bpg_g[index].GetBPM() == nullptr);
    assert(rpg.GetPage() == p_g[index].get() && rpg.GetBPM() == bpm);
  }

  // Clean the resource
  delete bpm;

  // Shutdown the Disk Manager
  disk_manager->ShutDown();
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
