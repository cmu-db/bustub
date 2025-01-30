//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// extendible_htable_concurrent_test.cpp
//
// Identification: test/container/disk/hash/extendible_htable_concurrent_test.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <thread>  // NOLINT
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "common/logger.h"
#include "container/disk/hash/disk_extendible_hash_table.h"
#include "gtest/gtest.h"
#include "murmur3/MurmurHash3.h"
#include "storage/disk/disk_manager_memory.h"
#include "test_util.h"  // NOLINT

namespace bustub {

using bustub::DiskManagerUnlimitedMemory;

//===----------------------------------------------------------------------===//
// helper function to launch multiple threads
template <typename... Args>
void LaunchParallelTest(uint64_t num_threads, Args &&...args) {
  std::vector<std::thread> thread_group;

  // Launch a group of threads
  for (uint64_t thread_itr = 0; thread_itr < num_threads; ++thread_itr) {
    thread_group.push_back(std::thread(args..., thread_itr));
  }

  // Join the threads with the main thread
  for (uint64_t thread_itr = 0; thread_itr < num_threads; ++thread_itr) {
    thread_group[thread_itr].join();
  }
}

// helper function to insert
void InsertHelper(DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> *ht,
                  const std::vector<int64_t> &keys, __attribute__((unused)) uint64_t thread_itr = 0) {
  GenericKey<8> index_key;
  RID rid;
  for (auto key : keys) {
    int64_t value = key & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(key >> 32), value);
    index_key.SetFromInteger(key);
    ht->Insert(index_key, rid);
  }
}

// helper function to seperate insert
void InsertHelperSplit(DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> *ht,
                       const std::vector<int64_t> &keys, int total_threads,
                       __attribute__((unused)) uint64_t thread_itr) {
  GenericKey<8> index_key;
  RID rid;
  for (auto key : keys) {
    if (static_cast<uint64_t>(key) % total_threads == thread_itr) {
      int64_t value = key & 0xFFFFFFFF;
      rid.Set(static_cast<int32_t>(key >> 32), value);
      index_key.SetFromInteger(key);
      ht->Insert(index_key, rid);
    }
  }
}

// helper function to delete
void DeleteHelper(DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> *ht,
                  const std::vector<int64_t> &remove_keys, __attribute__((unused)) uint64_t thread_itr = 0) {
  GenericKey<8> index_key;
  for (auto key : remove_keys) {
    index_key.SetFromInteger(key);
    ht->Remove(index_key);
  }
}

// helper function to seperate delete
void DeleteHelperSplit(DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> *ht,
                       const std::vector<int64_t> &remove_keys, int total_threads,
                       __attribute__((unused)) uint64_t thread_itr) {
  GenericKey<8> index_key;
  for (auto key : remove_keys) {
    if (static_cast<uint64_t>(key) % total_threads == thread_itr) {
      index_key.SetFromInteger(key);
      ht->Remove(index_key);
    }
  }
}

void LookupHelper(DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> *ht,
                  const std::vector<int64_t> &keys, uint64_t tid, __attribute__((unused)) uint64_t thread_itr = 0) {
  GenericKey<8> index_key;
  RID rid;
  for (auto key : keys) {
    int64_t value = key & 0xFFFFFFFF;
    rid.Set(static_cast<int32_t>(key >> 32), value);
    index_key.SetFromInteger(key);
    std::vector<RID> result;
    bool res = ht->GetValue(index_key, &result);
    ASSERT_EQ(res, true);
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0], rid);
  }
}

//===----------------------------------------------------------------------===//

// NOLINTNEXTLINE
TEST(ExtendibleHTableConcurrentTest, DISABLED_InsertTest1) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  // create hash table
  DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> ht("blah", bpm.get(), comparator,
                                                                       HashFunction<GenericKey<8>>());

  // keys to Insert
  std::vector<int64_t> keys;
  int64_t scale_factor = 100;
  for (int64_t key = 1; key < scale_factor; key++) {
    keys.push_back(key);
  }
  LaunchParallelTest(2, InsertHelper, &ht, keys);

  std::vector<RID> rids;
  GenericKey<8> index_key;
  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    ht.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 1);

    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }
}

// NOLINTNEXTLINE
TEST(ExtendibleHTableConcurrentTest, DISABLED_InsertTest2) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());
  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  // create hash table
  DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> ht("blah", bpm.get(), comparator,
                                                                       HashFunction<GenericKey<8>>());

  // keys to Insert
  std::vector<int64_t> keys;
  int64_t scale_factor = 100;
  for (int64_t key = 1; key < scale_factor; key++) {
    keys.push_back(key);
  }
  LaunchParallelTest(2, InsertHelperSplit, &ht, keys, 2);

  std::vector<RID> rids;
  GenericKey<8> index_key;
  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    ht.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 1);

    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }
}

// NOLINTNEXTLINE
TEST(ExtendibleHTableConcurrentTest, DISABLED_DeleteTest1) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  // create hash table
  DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> ht("blah", bpm.get(), comparator,
                                                                       HashFunction<GenericKey<8>>());
  // sequential insert
  std::vector<int64_t> keys = {1, 2, 3, 4, 5};
  InsertHelper(&ht, keys);

  std::vector<int64_t> remove_keys = {1, 5, 3, 4};
  LaunchParallelTest(2, DeleteHelper, &ht, remove_keys);

  std::vector<RID> rids;
  GenericKey<8> index_key;
  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    ht.GetValue(index_key, &rids);
    if (key != 2) {
      EXPECT_EQ(rids.size(), 0);
      continue;
    }
    EXPECT_EQ(rids.size(), 1);

    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }
}

// NOLINTNEXTLINE
TEST(ExtendibleHTableConcurrentTest, DISABLED_DeleteTest2) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  // create hash table
  DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> ht("blah", bpm.get(), comparator,
                                                                       HashFunction<GenericKey<8>>());

  // sequential insert
  std::vector<int64_t> keys = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  InsertHelper(&ht, keys);

  std::vector<int64_t> remove_keys = {1, 4, 3, 2, 5, 6};
  LaunchParallelTest(2, DeleteHelperSplit, &ht, remove_keys, 2);

  std::vector<RID> rids;
  GenericKey<8> index_key;
  for (auto key : keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    ht.GetValue(index_key, &rids);
    if (key <= 6) {
      EXPECT_EQ(rids.size(), 0);
      continue;
    }
    EXPECT_EQ(rids.size(), 1);

    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }
}

TEST(ExtendibleHTableConcurrentTest, DISABLED_MixTest1) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  // create hash table
  DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> ht("blah", bpm.get(), comparator,
                                                                       HashFunction<GenericKey<8>>());

  // first, populate index
  std::vector<int64_t> keys = {1, 2, 3, 4, 5};
  InsertHelper(&ht, keys);

  // concurrent insert
  keys.clear();
  for (int i = 6; i <= 10; i++) {
    keys.push_back(i);
  }
  LaunchParallelTest(1, InsertHelper, &ht, keys);
  // concurrent delete
  std::vector<int64_t> remove_keys = {1, 4, 3, 5, 6};
  LaunchParallelTest(1, DeleteHelper, &ht, remove_keys);

  std::vector<int64_t> valid_keys = {2, 7, 8, 9, 10};
  std::vector<int64_t> invalid_keys = {1, 3, 4, 5, 6};

  std::vector<RID> rids;
  GenericKey<8> index_key;
  for (auto key : valid_keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    ht.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 1);
    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }
  for (auto key : invalid_keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    ht.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 0);
  }
}

TEST(ExtendibleHTableConcurrentTest, DISABLED_MixTest2) {
  // create KeyComparator and index schema
  auto key_schema = ParseCreateStatement("a bigint");
  GenericComparator<8> comparator(key_schema.get());

  auto disk_mgr = std::make_unique<DiskManagerUnlimitedMemory>();
  auto bpm = std::make_unique<BufferPoolManager>(50, disk_mgr.get());

  // create hash table
  DiskExtendibleHashTable<GenericKey<8>, RID, GenericComparator<8>> ht("blah", bpm.get(), comparator,
                                                                       HashFunction<GenericKey<8>>());

  // Add preserved_keys
  std::vector<int64_t> preserved_keys;
  std::vector<int64_t> dynamic_keys;
  int64_t total_keys = 50;
  int64_t sieve = 5;
  for (int64_t i = 1; i <= total_keys; i++) {
    if (i % sieve == 0) {
      preserved_keys.push_back(i);
    } else {
      dynamic_keys.push_back(i);
    }
  }
  InsertHelper(&ht, preserved_keys, 1);

  auto insert_task = [&](int tid) { InsertHelper(&ht, dynamic_keys, tid); };
  auto delete_task = [&](int tid) { DeleteHelper(&ht, dynamic_keys, tid); };
  auto lookup_task = [&](int tid) { LookupHelper(&ht, preserved_keys, tid); };

  std::vector<std::thread> threads;
  std::vector<std::function<void(int)>> tasks;
  tasks.emplace_back(insert_task);
  tasks.emplace_back(delete_task);
  tasks.emplace_back(lookup_task);

  size_t num_threads = 6;
  for (size_t i = 0; i < num_threads; i++) {
    threads.emplace_back(std::thread{tasks[i % tasks.size()], i});
  }
  for (size_t i = 0; i < num_threads; i++) {
    threads[i].join();
  }

  // Check all preserved keys exist
  std::vector<RID> rids;
  GenericKey<8> index_key;
  for (auto key : preserved_keys) {
    rids.clear();
    index_key.SetFromInteger(key);
    ht.GetValue(index_key, &rids);
    EXPECT_EQ(rids.size(), 1);
    int64_t value = key & 0xFFFFFFFF;
    EXPECT_EQ(rids[0].GetSlotNum(), value);
  }
}

}  // namespace bustub
