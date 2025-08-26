//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree_delete_test.cpp
//
// Identification: test/storage/b_plus_tree_delete_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <cstdio>

#include "buffer/buffer_pool_manager.h"
#include "gtest/gtest.h"
#include "storage/b_plus_tree_utils.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/index/b_plus_tree.h"
#include "test_util.h"  // NOLINT

namespace bustub {

using bustub::DiskManagerUnlimitedMemory;

TEST(BPlusTreeTests, DISABLED_DeleteTestNoIterator) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto disk_manager = std::make_unique<DiskManagerUnlimitedMemory>();
  auto *bpm = new BufferPoolManager(50, disk_manager.get());
  // allocate header_page
  page_id_t page_id = bpm->NewPage();
  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", page_id, bpm, comparator, 2, 3);
  GenericKey<8> index_key;
  RID rid;

  std::vector<int64_t> keys = {1, 2, 3, 4, 5};
  for (auto key : keys) {
    int64_t value = key & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(key >> 32), value);
    index_key.SetFromInteger(key);
    tree.Insert(index_key, rid);
  }

  std::vector<RID> rids;
  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    tree.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 1);

    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }

  std::vector<int64_t> remove_keys = {1, 5, 3, 4};
  for (auto key : remove_keys) {
    index_key.SetFromInteger(key);
    tree.Remove(index_key);
  }

  int64_t size = 0;
  bool is_present;

  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    is_present = tree.GetValue(index_key, &rids);

    if (!is_present) {
      EXPECT_NE(std::find(remove_keys.begin(), remove_keys.end(), key), remove_keys.end());
    } else {
      EXPECT_EQ(rids.size(), 1);
      EXPECT_EQ(rids[0].GetPageId(), 0);
      EXPECT_EQ(rids[0].GetSlotNum(), key);
      ++size;
    }
  }
  EXPECT_EQ(size, 1);

  // Remove the remaining key
  index_key.SetFromInteger(2);
  tree.Remove(index_key);
  auto root_page_id = tree.GetRootPageId();
  ASSERT_EQ(root_page_id, INVALID_PAGE_ID);

  delete bpm;
}

TEST(BPlusTreeTests, DISABLED_SequentialEdgeMixTest) {  // NOLINT
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto disk_manager = std::make_unique<DiskManagerUnlimitedMemory>();
  auto *bpm = new BufferPoolManager(50, disk_manager.get());

  for (int leaf_max_size = 2; leaf_max_size <= 5; leaf_max_size++) {
    // create and fetch header_page
    page_id_t page_id = bpm->NewPage();

    // create b+ tree
    BPlusTree<GenericKey<8>, RID, GenericComparator<8>> tree("foo_pk", page_id, bpm, comparator, leaf_max_size, 3);
    GenericKey<8> index_key;
    RID rid;

    std::vector<int64_t> keys = {1, 5, 15, 20, 25, 2, -1, -2, 6, 14, 4};
    std::vector<int64_t> inserted = {};
    std::vector<int64_t> deleted = {};
    for (auto key : keys) {
      int64_t value = key & 0xFFFFFFFF;
      rid.Set(static_cast<int32_t>(key >> 32), value);
      index_key.SetFromInteger(key);
      tree.Insert(index_key, rid);
      inserted.push_back(key);
      auto res = TreeValuesMatch<GenericKey<8>, RID, GenericComparator<8>>(tree, inserted, deleted);
      ASSERT_TRUE(res);
    }

    index_key.SetFromInteger(1);
    tree.Remove(index_key);
    deleted.push_back(1);
    inserted.erase(std::find(inserted.begin(), inserted.end(), 1));
    auto res = TreeValuesMatch<GenericKey<8>, RID, GenericComparator<8>>(tree, inserted, deleted);
    ASSERT_TRUE(res);

    index_key.SetFromInteger(3);
    rid.Set(3, 3);
    tree.Insert(index_key, rid);
    inserted.push_back(3);
    res = TreeValuesMatch<GenericKey<8>, RID, GenericComparator<8>>(tree, inserted, deleted);
    ASSERT_TRUE(res);

    keys = {4, 14, 6, 2, 15, -2, -1, 3, 5, 25, 20};
    for (auto key : keys) {
      index_key.SetFromInteger(key);
      tree.Remove(index_key);
      deleted.push_back(key);
      inserted.erase(std::find(inserted.begin(), inserted.end(), key));
      res = TreeValuesMatch<GenericKey<8>, RID, GenericComparator<8>>(tree, inserted, deleted);
      ASSERT_TRUE(res);
    }
  }

  delete bpm;
}
}  // namespace bustub
