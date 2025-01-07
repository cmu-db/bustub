//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_page_test.cpp
//
// Identification: test/storage/extendible_htable_page_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <memory>
#include <thread>  // NOLINT
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "common/logger.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/index/generic_key.h"
#include "storage/index/int_comparator.h"
#include "storage/page/extendible_htable_bucket_page.h"
#include "storage/page/extendible_htable_directory_page.h"
#include "storage/page/extendible_htable_header_page.h"
#include "storage/page/page_guard.h"
#include "test_util.h"  // NOLINT

namespace bustub {

// NOLINTNEXTLINE
TEST(ExtendibleHTableTest, DISABLED_BucketPageSampleTest) {
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(5, disk_mgr.get());

  page_id_t bucket_page_id = bpm->NewPage();
  auto guard = bpm->WritePage(bucket_page_id);
  auto bucket_page = guard.AsMut<ExtendibleHTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>>();
  bucket_page->Init(10);

  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());
  GenericKey<8> index_key;
  RID rid;

  // insert a few (key, value) pairs
  for (int64_t i = 0; i < 10; i++) {
    index_key.SetFromInteger(i);
    rid.Set(i, i);
    ASSERT_TRUE(bucket_page->Insert(index_key, rid, comparator));
  }

  index_key.SetFromInteger(11);
  rid.Set(11, 11);
  ASSERT_TRUE(bucket_page->IsFull());
  ASSERT_FALSE(bucket_page->Insert(index_key, rid, comparator));

  // check for the inserted pairs
  for (unsigned i = 0; i < 10; i++) {
    index_key.SetFromInteger(i);
    ASSERT_TRUE(bucket_page->Lookup(index_key, rid, comparator));
    ASSERT_EQ(rid, RID(i, i));
  }

  // remove a few pairs
  for (unsigned i = 0; i < 10; i++) {
    if (i % 2 == 1) {
      index_key.SetFromInteger(i);
      ASSERT_TRUE(bucket_page->Remove(index_key, comparator));
    }
  }

  for (unsigned i = 0; i < 10; i++) {
    if (i % 2 == 1) {
      // remove the same pairs again
      index_key.SetFromInteger(i);
      ASSERT_FALSE(bucket_page->Remove(index_key, comparator));
    } else {
      index_key.SetFromInteger(i);
      ASSERT_TRUE(bucket_page->Remove(index_key, comparator));
    }
  }

  ASSERT_TRUE(bucket_page->IsEmpty());
  // page guard dropped
}

// NOLINTNEXTLINE
TEST(ExtendibleHTableTest, DISABLED_HeaderDirectoryPageSampleTest) {
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(5, disk_mgr.get());

  /************************ HEADER PAGE TEST ************************/
  page_id_t header_page_id = bpm->NewPage();
  auto header_guard = bpm->WritePage(header_page_id);
  auto header_page = header_guard.AsMut<ExtendibleHTableHeaderPage>();
  header_page->Init(2);

  /* Test hashes for header page
  00000000000000001000000000000000 - 32768
  01000000000000001000000000000000 - 1073774592
  10000000000000001000000000000000 - 2147516416
  11000000000000001000000000000000 - 3221258240
  */

  // ensure we are hashing into proper bucket based on upper 2 bits
  uint32_t hashes[4]{32768, 1073774592, 2147516416, 3221258240};
  for (uint32_t i = 0; i < 4; i++) {
    ASSERT_EQ(header_page->HashToDirectoryIndex(hashes[i]), i);
  }

  header_guard.Drop();

  /************************ DIRECTORY PAGE TEST ************************/
  page_id_t directory_page_id = bpm->NewPage();
  auto directory_guard = bpm->WritePage(directory_page_id);
  auto directory_page = directory_guard.AsMut<ExtendibleHTableDirectoryPage>();
  directory_page->Init(3);

  page_id_t bucket_page_id_1 = bpm->NewPage();
  auto bucket_guard_1 = bpm->WritePage(bucket_page_id_1);
  auto bucket_page_1 = bucket_guard_1.AsMut<ExtendibleHTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>>();
  bucket_page_1->Init(10);

  page_id_t bucket_page_id_2 = bpm->NewPage();
  auto bucket_guard_2 = bpm->WritePage(bucket_page_id_2);
  auto bucket_page_2 = bucket_guard_2.AsMut<ExtendibleHTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>>();
  bucket_page_2->Init(10);

  page_id_t bucket_page_id_3 = bpm->NewPage();
  auto bucket_guard_3 = bpm->WritePage(bucket_page_id_3);
  auto bucket_page_3 = bucket_guard_3.AsMut<ExtendibleHTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>>();
  bucket_page_3->Init(10);

  page_id_t bucket_page_id_4 = bpm->NewPage();
  auto bucket_guard_4 = bpm->WritePage(bucket_page_id_4);
  auto bucket_page_4 = bucket_guard_4.AsMut<ExtendibleHTableBucketPage<GenericKey<8>, RID, GenericComparator<8>>>();
  bucket_page_4->Init(10);

  directory_page->SetBucketPageId(0, bucket_page_id_1);

  /*
  ======== DIRECTORY (global_depth_: 0) ========
  | bucket_idx | page_id | local_depth |
  |    0    |    2    |    0    |
  ================ END DIRECTORY ================
  */

  directory_page->VerifyIntegrity();
  ASSERT_EQ(directory_page->Size(), 1);
  ASSERT_EQ(directory_page->GetBucketPageId(0), bucket_page_id_1);

  // grow the directory, local depths should change!
  directory_page->SetLocalDepth(0, 1);
  directory_page->IncrGlobalDepth();
  directory_page->SetBucketPageId(1, bucket_page_id_2);
  directory_page->SetLocalDepth(1, 1);

  /*
  ======== DIRECTORY (global_depth_: 1) ========
  | bucket_idx | page_id | local_depth |
  |    0    |    2    |    1    |
  |    1    |    3    |    1    |
  ================ END DIRECTORY ================
  */

  directory_page->VerifyIntegrity();
  ASSERT_EQ(directory_page->Size(), 2);
  ASSERT_EQ(directory_page->GetBucketPageId(0), bucket_page_id_1);
  ASSERT_EQ(directory_page->GetBucketPageId(1), bucket_page_id_2);

  for (uint32_t i = 0; i < 100; i++) {
    ASSERT_EQ(directory_page->HashToBucketIndex(i), i % 2);
  }

  directory_page->SetLocalDepth(0, 2);
  directory_page->IncrGlobalDepth();
  directory_page->SetBucketPageId(2, bucket_page_id_3);

  /*
  ======== DIRECTORY (global_depth_: 2) ========
  | bucket_idx | page_id | local_depth |
  |    0    |    2    |    2    |
  |    1    |    3    |    1    |
  |    2    |    4    |    2    |
  |    3    |    3    |    1    |
  ================ END DIRECTORY ================
  */

  directory_page->VerifyIntegrity();
  ASSERT_EQ(directory_page->Size(), 4);
  ASSERT_EQ(directory_page->GetBucketPageId(0), bucket_page_id_1);
  ASSERT_EQ(directory_page->GetBucketPageId(1), bucket_page_id_2);
  ASSERT_EQ(directory_page->GetBucketPageId(2), bucket_page_id_3);
  ASSERT_EQ(directory_page->GetBucketPageId(3), bucket_page_id_2);

  for (uint32_t i = 0; i < 100; i++) {
    ASSERT_EQ(directory_page->HashToBucketIndex(i), i % 4);
  }

  directory_page->SetLocalDepth(0, 3);
  directory_page->IncrGlobalDepth();
  directory_page->SetBucketPageId(4, bucket_page_id_4);

  /*
  ======== DIRECTORY (global_depth_: 3) ========
  | bucket_idx | page_id | local_depth |
  |    0    |    2    |    3    |
  |    1    |    3    |    1    |
  |    2    |    4    |    2    |
  |    3    |    3    |    1    |
  |    4    |    5    |    3    |
  |    5    |    3    |    1    |
  |    6    |    4    |    2    |
  |    7    |    3    |    1    |
  ================ END DIRECTORY ================
  */
  directory_page->VerifyIntegrity();
  ASSERT_EQ(directory_page->Size(), 8);
  ASSERT_EQ(directory_page->GetBucketPageId(0), bucket_page_id_1);
  ASSERT_EQ(directory_page->GetBucketPageId(1), bucket_page_id_2);
  ASSERT_EQ(directory_page->GetBucketPageId(2), bucket_page_id_3);
  ASSERT_EQ(directory_page->GetBucketPageId(3), bucket_page_id_2);
  ASSERT_EQ(directory_page->GetBucketPageId(4), bucket_page_id_4);
  ASSERT_EQ(directory_page->GetBucketPageId(5), bucket_page_id_2);
  ASSERT_EQ(directory_page->GetBucketPageId(6), bucket_page_id_3);
  ASSERT_EQ(directory_page->GetBucketPageId(7), bucket_page_id_2);

  for (uint32_t i = 0; i < 100; i++) {
    ASSERT_EQ(directory_page->HashToBucketIndex(i), i % 8);
  }

  // uncommenting this code line below should cause an "Assertion failed"
  // since this would be exceeding the max depth we initialized
  // directory_page->IncrGlobalDepth();

  // at this time, we cannot shrink the directory since we have ld = gd = 3
  ASSERT_EQ(directory_page->CanShrink(), false);

  directory_page->SetLocalDepth(0, 2);
  directory_page->SetLocalDepth(4, 2);
  directory_page->SetBucketPageId(0, bucket_page_id_4);

  /*
  ======== DIRECTORY (global_depth_: 3) ========
  | bucket_idx | page_id | local_depth |
  |    0    |    5    |    2    |
  |    1    |    3    |    1    |
  |    2    |    4    |    2    |
  |    3    |    3    |    1    |
  |    4    |    5    |    2    |
  |    5    |    3    |    1    |
  |    6    |    4    |    2    |
  |    7    |    3    |    1    |
  ================ END DIRECTORY ================
  */

  ASSERT_EQ(directory_page->CanShrink(), true);
  directory_page->DecrGlobalDepth();

  /*
  ======== DIRECTORY (global_depth_: 2) ========
  | bucket_idx | page_id | local_depth |
  |    0    |    5    |    2    |
  |    1    |    3    |    1    |
  |    2    |    4    |    2    |
  |    3    |    3    |    1    |
  ================ END DIRECTORY ================
  */

  directory_page->VerifyIntegrity();
  ASSERT_EQ(directory_page->Size(), 4);
  ASSERT_EQ(directory_page->CanShrink(), false);
  // page guard dropped
}

}  // namespace bustub
