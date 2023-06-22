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
#include <random>
#include <string>

#include "buffer/buffer_pool_manager.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/page/page_guard.h"

#include "gtest/gtest.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(PageGuardTest, DISABLED_GuardUpgradeTest) {
  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  const size_t buffer_pool_size = 5;
  const size_t k = 2;
  auto *bpm = new BufferPoolManager(buffer_pool_size, disk_manager.get(), k);

  // Create two dummy pages
  Page *dummy_page_1 = new Page();
  Page *dummy_page_2 = new Page();
  // Create two dummy BasicPageGuards
  auto b_pg_1 = BasicPageGuard(bpm, dummy_page_1);
  auto b_pg_2 = BasicPageGuard(bpm, dummy_page_2);

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
TEST(PageGuardTest, DISABLED_SampleTest) {
  const size_t buffer_pool_size = 5;
  const size_t k = 2;

  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_shared<BufferPoolManager>(buffer_pool_size, disk_manager.get(), k);

  page_id_t page_id_temp;
  auto *page0 = bpm->NewPage(&page_id_temp);

  auto guarded_page = BasicPageGuard(bpm.get(), page0);

  EXPECT_EQ(page0->GetData(), guarded_page.GetData());
  EXPECT_EQ(page0->GetPageId(), guarded_page.PageId());
  EXPECT_EQ(1, page0->GetPinCount());

  guarded_page.Drop();

  EXPECT_EQ(0, page0->GetPinCount());

  // Shutdown the disk manager and remove the temporary file we created.
  disk_manager->ShutDown();
}

}  // namespace bustub
