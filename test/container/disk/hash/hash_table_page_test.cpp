//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_page_test.cpp
//
// Identification: test/container/hash_table_page_test.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <thread>  // NOLINT
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "common/logger.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager.h"
#include "storage/page/hash_table_bucket_page.h"
#include "storage/page/hash_table_directory_page.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(HashTablePageTest, DISABLED_DirectoryPageSampleTest) {
  auto *disk_manager = new DiskManager("test.db");
  auto *bpm = new BufferPoolManager(5, disk_manager);

  // get a directory page from the BufferPoolManager
  page_id_t directory_page_id = INVALID_PAGE_ID;
  auto directory_page = reinterpret_cast<HashTableDirectoryPage *>(bpm->NewPage(&directory_page_id)->GetData());

  EXPECT_EQ(0, directory_page->GetGlobalDepth());
  directory_page->SetPageId(10);
  EXPECT_EQ(10, directory_page->GetPageId());
  directory_page->SetLSN(100);
  EXPECT_EQ(100, directory_page->GetLSN());

  // add a few hypothetical bucket pages
  for (unsigned i = 0; i < 8; i++) {
    directory_page->SetBucketPageId(i, i);
  }

  // check for correct bucket page IDs
  for (int i = 0; i < 8; i++) {
    EXPECT_EQ(i, directory_page->GetBucketPageId(i));
  }

  // unpin the directory page now that we are done
  bpm->UnpinPage(directory_page_id, true);
  disk_manager->ShutDown();
  remove("test.db");
  delete disk_manager;
  delete bpm;
}

// NOLINTNEXTLINE
TEST(HashTablePageTest, DISABLED_BucketPageSampleTest) {
  auto *disk_manager = new DiskManager("test.db");
  auto *bpm = new BufferPoolManager(5, disk_manager);

  // get a bucket page from the BufferPoolManager
  page_id_t bucket_page_id = INVALID_PAGE_ID;

  auto bucket_page =
      reinterpret_cast<HashTableBucketPage<int, int, IntComparator> *>(bpm->NewPage(&bucket_page_id)->GetData());

  // insert a few (key, value) pairs
  for (unsigned i = 0; i < 10; i++) {
    assert(bucket_page->Insert(i, i, IntComparator()));
  }

  // check for the inserted pairs
  for (unsigned i = 0; i < 10; i++) {
    EXPECT_EQ(i, bucket_page->KeyAt(i));
    EXPECT_EQ(i, bucket_page->ValueAt(i));
  }

  // remove a few pairs
  for (unsigned i = 0; i < 10; i++) {
    if (i % 2 == 1) {
      assert(bucket_page->Remove(i, i, IntComparator()));
    }
  }

  // check for the flags
  for (unsigned i = 0; i < 15; i++) {
    if (i < 10) {
      EXPECT_TRUE(bucket_page->IsOccupied(i));
      if (i % 2 == 1) {
        EXPECT_FALSE(bucket_page->IsReadable(i));
      } else {
        EXPECT_TRUE(bucket_page->IsReadable(i));
      }
    } else {
      EXPECT_FALSE(bucket_page->IsOccupied(i));
    }
  }

  // try to remove the already-removed pairs
  for (unsigned i = 0; i < 10; i++) {
    if (i % 2 == 1) {
      assert(!bucket_page->Remove(i, i, IntComparator()));
    }
  }

  // unpin the directory page now that we are done
  bpm->UnpinPage(bucket_page_id, true);
  disk_manager->ShutDown();
  remove("test.db");
  delete disk_manager;
  delete bpm;
}

}  // namespace bustub
