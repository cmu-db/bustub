//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_bucket_page_test.cpp
//
// Identification: test/storage/extendible_htable_bucket_page_test.cpp
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
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
#include "storage/page/page_guard.h"
#include "test_util.h"  // NOLINT

namespace bustub {

// NOLINTNEXTLINE
TEST(ExtendibleHTableTest, DISABLED_BucketPageSampleTest) {
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(5, disk_mgr.get());

  page_id_t bucket_page_id = INVALID_PAGE_ID;
  {
    BasicPageGuard guard = bpm->NewPageGuarded(&bucket_page_id);
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
  }  // page guard dropped
}

}  // namespace bustub
