
#include <algorithm>
#include "buffer/buffer_pool_manager.h"
#include "common/config.h"
#include "gtest/gtest.h"
#include "storage/b_plus_tree_utils.h"
#include "storage/disk/disk_manager_memory.h"
#include "storage/index/b_plus_tree.h"
#include "storage/index/generic_key.h"
#include "storage/page/b_plus_tree_leaf_page.h"
#include "test_util.h"  // NOLINT

namespace bustub {

using bustub::DiskManagerUnlimitedMemory;

TEST(BPlusTreeTests, DISABLED_TombstoneBasicTest) {
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto *disk_manager = new DiskManagerUnlimitedMemory();
  auto *bpm = new BufferPoolManager(50, disk_manager);

  // create and fetch header_page
  page_id_t page_id = bpm->NewPage();

  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>, 2> tree("foo_pk", page_id, bpm, comparator, 4, 4);
  GenericKey<8> index_key;
  RID rid;

  size_t num_keys = 17;
  std::vector<size_t> expected;
  for (size_t i = 0; i < num_keys; i++) {
    int64_t value = i & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(i >> 32), value);
    index_key.SetFromInteger(i);
    tree.Insert(index_key, rid);
    expected.push_back(i);
  }

  // Test tombstones are being used / affect the index iterator correctly

  std::vector<int64_t> to_delete = {1, 5, 9};
  for (auto i : to_delete) {
    index_key.SetFromInteger(i);
    tree.Remove(index_key);
    expected.erase(std::remove(expected.begin(), expected.end(), i), expected.end());
  }

  size_t i = 0;
  for (auto iter = tree.Begin(); iter != tree.End(); ++iter) {
    ASSERT_EQ((*iter).first.GetAsInteger(), expected[i]);
    i++;
  }

  std::vector<int64_t> tombstones;
  auto leaf = IndexLeaves<GenericKey<8>, RID, GenericComparator<8>, 2>(tree.GetRootPageId(), bpm);
  while (leaf.Valid()) {
    for (auto t : (*leaf)->GetTombstones()) {
      tombstones.push_back(t.GetAsInteger());
    }
    ++leaf;
  }

  EXPECT_EQ(tombstones.size(), to_delete.size());
  for (size_t i = 0; i < tombstones.size(); i++) {
    EXPECT_EQ(tombstones[i], to_delete[i]);
  }

  // Test insertions interact correctly with tombstones

  for (auto i : to_delete) {
    int64_t value = (2 * i) & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(i >> 32), value);
    index_key.SetFromInteger(i);
    tree.Insert(index_key, rid);
  }

  leaf = IndexLeaves<GenericKey<8>, RID, GenericComparator<8>, 2>(tree.GetRootPageId(), bpm);
  while (leaf.Valid()) {
    EXPECT_EQ((*leaf)->GetTombstones().size(), 0);
    ++leaf;
  }

  for (auto i : to_delete) {
    index_key.SetFromInteger(i);
    std::vector<RID> rids;
    tree.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 1);
    EXPECT_EQ(rids[0].GetSlotNum(), (2 * i) & 0xFFFFFFFF);
  }

  // Test tombstones are processed in the correct order

  to_delete.clear();
  {
    auto leaf = IndexLeaves<GenericKey<8>, RID, GenericComparator<8>, 2>(tree.GetRootPageId(), bpm);
    while (leaf.Valid()) {
      EXPECT_EQ(2, (*leaf)->GetMinSize());
      if ((*leaf)->GetSize() > (*leaf)->GetMinSize()) {
        for (int i = 0; i < (*leaf)->GetMinSize() + 1; i++) {
          to_delete.push_back((*leaf)->KeyAt(i).GetAsInteger());
        }
        break;
      }
      ++leaf;
    }
  }

  for (auto i : to_delete) {
    index_key.SetFromInteger(i);
    tree.Remove(index_key);
  }

  tombstones.clear();
  leaf = IndexLeaves<GenericKey<8>, RID, GenericComparator<8>, 2>(tree.GetRootPageId(), bpm);
  while (leaf.Valid()) {
    for (auto t : (*leaf)->GetTombstones()) {
      tombstones.push_back(t.GetAsInteger());
    }
    ++leaf;
  }
  EXPECT_EQ(tombstones.size(), to_delete.size() - 1);
  for (size_t i = 0; i < tombstones.size(); i++) {
    EXPECT_EQ(tombstones[i], to_delete[i + 1]);
  }

  std::vector<RID> rids;
  index_key.SetFromInteger(to_delete[0]);
  tree.GetValue(index_key, &rids);
  EXPECT_EQ(rids.size(), 0);

  // Test index iterator stays valid for "empty" tree (and that tree isn't fully physically deleted)

  for (size_t i = 0; i < num_keys; i++) {
    index_key.SetFromInteger(i);
    tree.Remove(index_key);
  }

  leaf = IndexLeaves<GenericKey<8>, RID, GenericComparator<8>, 2>(tree.GetRootPageId(), bpm);
  size_t tot_tombs = 0;
  while (leaf.Valid()) {
    tot_tombs += (*leaf)->GetTombstones().size();
    ++leaf;
  }

  // Worst case: all keys are in full leaf nodes and so only 2 entries are tombed per.
  EXPECT_GT(tot_tombs, ((num_keys - 1) / 4) * 2);
  EXPECT_LT(tot_tombs, num_keys);
  EXPECT_EQ(tree.Begin().IsEnd(), true);

  delete bpm;
  delete disk_manager;
}

auto GetNumLeaves(BPlusTree<GenericKey<8>, RID, GenericComparator<8>, 3> &tree, BufferPoolManager *bpm) -> size_t {
  auto leaf = IndexLeaves<GenericKey<8>, RID, GenericComparator<8>, 3>(tree.GetRootPageId(), bpm);
  size_t count = 0;
  while (leaf.Valid()) {
    count++;
    ++leaf;
  }
  return count;
}

TEST(BPlusTreeTests, DISABLED_TombstoneSplitTest) {
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto *disk_manager = new DiskManagerUnlimitedMemory();
  auto *bpm = new BufferPoolManager(50, disk_manager);

  // create and fetch header_page
  page_id_t page_id = bpm->NewPage();

  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>, 3> tree("foo_pk", page_id, bpm, comparator, 5, 4);
  GenericKey<8> index_key;
  RID rid;

  for (size_t i = 0; i < 4; i++) {
    int64_t value = i & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(i >> 32), value);
    index_key.SetFromInteger(i);
    tree.Insert(index_key, rid);
  }

  index_key.SetFromInteger(3);
  tree.Remove(index_key);

  index_key.SetFromInteger(2);
  tree.Remove(index_key);

  index_key.SetFromInteger(0);
  tree.Remove(index_key);

  // Don't know when split will happen, so try both.
  size_t i = 4;
  while (GetNumLeaves(tree, bpm) < 2 && i < 6) {
    int64_t value = i & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(i >> 32), value);
    index_key.SetFromInteger(i);
    tree.Insert(index_key, rid);
    i++;
  }

  auto leaf = IndexLeaves<GenericKey<8>, RID, GenericComparator<8>, 3>(tree.GetRootPageId(), bpm);
  while (leaf.Valid()) {
    std::vector<size_t> expected;
    for (int i = 0; i < (*leaf)->GetSize(); i++) {
      auto key = (*leaf)->KeyAt(i).GetAsInteger();
      if (key == 0 || key == 2 || key == 3) {
        expected.push_back(key);
      }
    }
    std::sort(expected.rbegin(), expected.rend());
    auto tombstones = (*leaf)->GetTombstones();
    EXPECT_EQ(tombstones.size(), expected.size());
    for (size_t i = 0; i < tombstones.size(); i++) {
      EXPECT_EQ(tombstones[i].GetAsInteger(), expected[i]);
    }
    ++leaf;
  }

  delete bpm;
  delete disk_manager;
}

TEST(BPlusTreeTests, DISABLED_TombstoneBorrowTest) {
  using LeafPage = BPlusTreeLeafPage<GenericKey<8>, RID, GenericComparator<8>, 1>;
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto *disk_manager = new DiskManagerUnlimitedMemory();
  auto *bpm = new BufferPoolManager(50, disk_manager);

  // create and fetch header_page
  page_id_t page_id = bpm->NewPage();

  // create b+ tree
  BPlusTree<GenericKey<8>, RID, GenericComparator<8>, 1> tree("foo_pk", page_id, bpm, comparator, 4, 4);
  GenericKey<8> index_key;
  RID rid;

  size_t num_keys = 5;
  for (size_t i = 0; i < num_keys; i++) {
    int64_t value = i & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(i >> 32), value);
    index_key.SetFromInteger(i);
    tree.Insert(index_key, rid);
  }

  auto left_pid =
	GetLeftMostLeafPageId<GenericKey<8>, RID, GenericComparator<8>>(tree.GetRootPageId(), bpm);
  auto left_page = bpm->ReadPage(left_pid).As<LeafPage>();
  EXPECT_NE(left_page->GetNextPageId(), INVALID_PAGE_ID);
  auto right_page = bpm->ReadPage(left_page->GetNextPageId()).As<LeafPage>();

  std::vector<GenericKey<8>> to_remove;
  if (left_page->GetSize() == left_page->GetMinSize()) {
    to_remove.push_back(right_page->KeyAt(0));
    to_remove.push_back(left_page->KeyAt(1));
    to_remove.push_back(left_page->KeyAt(0));
  } else {
    to_remove.push_back(left_page->KeyAt(0));
    to_remove.push_back(right_page->KeyAt(1));
    to_remove.push_back(right_page->KeyAt(0));
  }

  for (auto k : to_remove) {
    tree.Remove(k);
  }

  std::vector<int64_t> tombstones;
  auto leaf = IndexLeaves<GenericKey<8>, RID, GenericComparator<8>, 1>(tree.GetRootPageId(), bpm);
  while (leaf.Valid()) {
    EXPECT_GE((*leaf)->GetSize(), (*leaf)->GetMinSize());
    for (auto t : (*leaf)->GetTombstones()) {
      tombstones.push_back(t.GetAsInteger());
    }
    ++leaf;
  }

  EXPECT_EQ(tombstones.size(), 1);
  EXPECT_EQ(tombstones[0], to_remove[0].GetAsInteger());

  delete bpm;
  delete disk_manager;
}
}  // namespace bustub
