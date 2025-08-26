//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// page_guard_test.cpp
//
// Identification: test/storage/page_guard_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <cstdio>

#include "buffer/buffer_pool_manager.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/page/page_guard.h"

#include "gtest/gtest.h"

namespace bustub {

const size_t FRAMES = 10;
const size_t K_DIST = 2;

TEST(PageGuardTest, DISABLED_DropTest) {
  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_shared<BufferPoolManager>(FRAMES, disk_manager.get(), K_DIST);

  {
    const auto pid0 = bpm->NewPage();
    auto page0 = bpm->WritePage(pid0);

    // The page should be pinned.
    ASSERT_EQ(1, bpm->GetPinCount(pid0));

    // A drop should unpin the page.
    page0.Drop();
    ASSERT_EQ(0, bpm->GetPinCount(pid0));

    // Another drop should have no effect.
    page0.Drop();
    ASSERT_EQ(0, bpm->GetPinCount(pid0));
  }  // Destructor should be called. Useless but should not cause issues.

  const auto pid1 = bpm->NewPage();
  const auto pid2 = bpm->NewPage();

  {
    auto read_guarded_page = bpm->ReadPage(pid1);
    auto write_guarded_page = bpm->WritePage(pid2);

    ASSERT_EQ(1, bpm->GetPinCount(pid1));
    ASSERT_EQ(1, bpm->GetPinCount(pid2));

    // Dropping should unpin the pages.
    read_guarded_page.Drop();
    write_guarded_page.Drop();
    ASSERT_EQ(0, bpm->GetPinCount(pid1));
    ASSERT_EQ(0, bpm->GetPinCount(pid2));

    // Another drop should have no effect.
    read_guarded_page.Drop();
    write_guarded_page.Drop();
    ASSERT_EQ(0, bpm->GetPinCount(pid1));
    ASSERT_EQ(0, bpm->GetPinCount(pid2));
  }  // Destructor should be called. Useless but should not cause issues.

  // This will hang if the latches were not unlocked correctly in the destructors.
  {
    const auto write_test1 = bpm->WritePage(pid1);
    const auto write_test2 = bpm->WritePage(pid2);
  }

  std::vector<page_id_t> page_ids;
  {
    // Fill up the BPM.
    std::vector<WritePageGuard> guards;
    for (size_t i = 0; i < FRAMES; i++) {
      const auto new_pid = bpm->NewPage();
      guards.push_back(bpm->WritePage(new_pid));
      ASSERT_EQ(1, bpm->GetPinCount(new_pid));
      page_ids.push_back(new_pid);
    }
  }  // This drops all of the guards.

  for (size_t i = 0; i < FRAMES; i++) {
    ASSERT_EQ(0, bpm->GetPinCount(page_ids[i]));
  }

  // Get a new write page and edit it. We will retrieve it later
  const auto mutable_page_id = bpm->NewPage();
  auto mutable_guard = bpm->WritePage(mutable_page_id);
  strcpy(mutable_guard.GetDataMut(), "data");  // NOLINT
  mutable_guard.Drop();

  {
    // Fill up the BPM again.
    std::vector<WritePageGuard> guards;
    for (size_t i = 0; i < FRAMES; i++) {
      auto new_pid = bpm->NewPage();
      guards.push_back(bpm->WritePage(new_pid));
      ASSERT_EQ(1, bpm->GetPinCount(new_pid));
    }
  }

  // Fetching the flushed page should result in seeing the changed value.
  auto immutable_guard = bpm->ReadPage(mutable_page_id);
  ASSERT_EQ(0, std::strcmp("data", immutable_guard.GetData()));

  // Shutdown the disk manager and remove the temporary file we created.
  disk_manager->ShutDown();
}

TEST(PageGuardTest, DISABLED_MoveTest) {
  auto disk_manager = std::make_shared<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_shared<BufferPoolManager>(FRAMES, disk_manager.get(), K_DIST);

  const auto pid0 = bpm->NewPage();
  const auto pid1 = bpm->NewPage();
  const auto pid2 = bpm->NewPage();
  const auto pid3 = bpm->NewPage();
  const auto pid4 = bpm->NewPage();
  const auto pid5 = bpm->NewPage();

  auto guard0 = bpm->ReadPage(pid0);
  auto guard1 = bpm->ReadPage(pid1);
  ASSERT_EQ(1, bpm->GetPinCount(pid0));
  ASSERT_EQ(1, bpm->GetPinCount(pid1));

  // This shouldn't change pin counts...
  auto &guard0_r = guard0;
  guard0 = std::move(guard0_r);
  ASSERT_EQ(1, bpm->GetPinCount(pid0));

  // Invalidate the old guard0 by move assignment.
  guard0 = std::move(guard1);
  ASSERT_EQ(0, bpm->GetPinCount(pid0));
  ASSERT_EQ(1, bpm->GetPinCount(pid1));

  // Invalidate the old guard0 by move construction.
  auto guard0a(std::move(guard0));
  ASSERT_EQ(0, bpm->GetPinCount(pid0));
  ASSERT_EQ(1, bpm->GetPinCount(pid1));

  auto guard2 = bpm->ReadPage(pid2);
  auto guard3 = bpm->ReadPage(pid3);
  ASSERT_EQ(1, bpm->GetPinCount(pid2));
  ASSERT_EQ(1, bpm->GetPinCount(pid3));

  // This shouldn't change pin counts...
  auto &guard2_r = guard2;
  guard2 = std::move(guard2_r);
  ASSERT_EQ(1, bpm->GetPinCount(pid2));

  // Invalidate the old guard3 by move assignment.
  guard2 = std::move(guard3);
  ASSERT_EQ(0, bpm->GetPinCount(pid2));
  ASSERT_EQ(1, bpm->GetPinCount(pid3));

  // Invalidate the old guard2 by move construction.
  auto guard2a(std::move(guard2));
  ASSERT_EQ(0, bpm->GetPinCount(pid2));
  ASSERT_EQ(1, bpm->GetPinCount(pid3));

  // This will hang if page 2 was not unlatched correctly.
  { const auto temp_guard2 = bpm->WritePage(pid2); }

  auto guard4 = bpm->WritePage(pid4);
  auto guard5 = bpm->WritePage(pid5);
  ASSERT_EQ(1, bpm->GetPinCount(pid4));
  ASSERT_EQ(1, bpm->GetPinCount(pid5));

  // This shouldn't change pin counts...
  auto &guard4_r = guard4;
  guard4 = std::move(guard4_r);
  ASSERT_EQ(1, bpm->GetPinCount(pid4));

  // Invalidate the old guard5 by move assignment.
  guard4 = std::move(guard5);
  ASSERT_EQ(0, bpm->GetPinCount(pid4));
  ASSERT_EQ(1, bpm->GetPinCount(pid5));

  // Invalidate the old guard4 by move construction.
  auto guard4a(std::move(guard4));
  ASSERT_EQ(0, bpm->GetPinCount(pid4));
  ASSERT_EQ(1, bpm->GetPinCount(pid5));

  // This will hang if page 4 was not unlatched correctly.
  { const auto temp_guard4 = bpm->ReadPage(pid4); }

  // Test move constructor with invalid that
  {
    ReadPageGuard invalidread0;
    const auto invalidread1{std::move(invalidread0)};
    WritePageGuard invalidwrite0;
    const auto invalidwrite1{std::move(invalidwrite0)};
  }

  // Test move assignment with invalid that
  {
    const auto pid = bpm->NewPage();
    auto read = bpm->ReadPage(pid);
    ReadPageGuard invalidread;
    read = std::move(invalidread);
    auto write = bpm->WritePage(pid);
    WritePageGuard invalidwrite;
    write = std::move(invalidwrite);
  }

  // Shutdown the disk manager and remove the temporary file we created.
  disk_manager->ShutDown();
}

}  // namespace bustub
