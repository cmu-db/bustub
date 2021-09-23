//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_test.cpp
//
// Identification: test/container/hash_table_test.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <thread>  // NOLINT
#include <vector>

#include "buffer/buffer_pool_manager_instance.h"
#include "common/logger.h"
#include "container/hash/extendible_hash_table.h"
#include "gtest/gtest.h"
#include "murmur3/MurmurHash3.h"

namespace bustub {

// NOLINTNEXTLINE
TEST(HashTableTest, SampleTest) {
  auto *disk_manager = new DiskManager("test.db");
  auto *bpm = new BufferPoolManagerInstance(50, disk_manager);
  ExtendibleHashTable<int, int, IntComparator> ht("blah", bpm, IntComparator(), HashFunction<int>());

  // insert a few values
  for (int i = 0; i < 5; i++) {
    EXPECT_TRUE(ht.Insert(nullptr, i, i));
    std::vector<int> res;
    EXPECT_TRUE(ht.GetValue(nullptr, i, &res));
    EXPECT_EQ(1, res.size()) << "Failed to insert " << i << std::endl;
    EXPECT_EQ(i, res[0]);
  }


  // check if the inserted values are all there
  for (int i = 0; i < 5; i++) {
    std::vector<int> res;
    EXPECT_TRUE(ht.GetValue(nullptr, i, &res));
    EXPECT_EQ(1, res.size()) << "Failed to keep " << i << std::endl;
    EXPECT_EQ(i, res[0]);
  }

  // insert one more value for each key
  for (int i = 0; i < 10; i++) {
    LOG_DEBUG("TEMP3");
    if (i < 5) {
      // duplicate values for the same key are not allowed
      EXPECT_FALSE(ht.Insert(nullptr, i, i));
      EXPECT_TRUE(ht.Insert(nullptr, i, i + 1));
    } else {
      EXPECT_TRUE(ht.Insert(nullptr, i, i));
      EXPECT_TRUE(ht.Insert(nullptr, i, i + 1));

      if (i % 2 == 0){
        EXPECT_TRUE(ht.Insert(nullptr, i, i + 2));
      }
    }
    EXPECT_FALSE(ht.Insert(nullptr, i, i));
    EXPECT_FALSE(ht.Insert(nullptr, i, i + 1));

    std::vector<int> res;
    ht.GetValue(nullptr, i, &res);

    if (i < 5 || i % 2 == 1) {
      // duplicate values for the same key are not allowed
      EXPECT_EQ(2, res.size());
      EXPECT_EQ(i, res[0]);
      EXPECT_EQ(i + 1, res[1]);
    } 
    else {
      EXPECT_EQ(3, res.size());
      EXPECT_EQ(i, res[0]);
      EXPECT_EQ(i + 1, res[1]);
      EXPECT_EQ(i + 2, res[2]);
    }
  }

  // look for a key that does not exist
  std::vector<int> res;
  EXPECT_FALSE(ht.GetValue(nullptr, 20, &res));
  EXPECT_EQ(0, res.size());

  // delete some values
  for (int i = 0; i < 10; i++) {
    EXPECT_TRUE(ht.Remove(nullptr, i, i));
    std::vector<int> res;
    EXPECT_TRUE(ht.GetValue(nullptr, i, &res));

    if (i < 5 || i % 2 == 1) {
      // (0, 0) is the only pair with key 0
      EXPECT_EQ(1, res.size());
      EXPECT_EQ(i + 1, res[0]);
    } else {
      EXPECT_EQ(2, res.size());
      EXPECT_EQ(i + 1, res[0]);
      EXPECT_EQ(i + 2, res[1]);
    }
  }

  // delete all values
  for (int i = 0; i < 10; i++) {
    EXPECT_FALSE(ht.Remove(nullptr, i, i));
  }
  disk_manager->ShutDown();
  remove("test.db");
  delete disk_manager;
  delete bpm;
}

}  // namespace bustub
