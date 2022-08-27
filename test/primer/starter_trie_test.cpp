//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// starter_test.cpp
//
// Identification: test/include/starter_test.cpp
//
// Copyright (c) 2015-2020, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <bitset>
#include <functional>
#include <numeric>
#include <random>
#include <thread>  // NOLINT

#include "common/exception.h"
#include "gtest/gtest.h"
#include "primer/p0_trie.h"

namespace bustub {

// Generate n random strings
std::vector<std::string> GenerateNRandomString(int n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<char> char_dist('A', 'z');
  std::uniform_int_distribution<int> len_dist(1, 30);

  std::vector<std::string> rand_strs(n);

  for (auto &rand_str : rand_strs) {
    int str_len = len_dist(gen);
    for (int i = 0; i < str_len; ++i) {
      rand_str.push_back(char_dist(gen));
    }
  }

  return rand_strs;
}

TEST(StarterTest, TrieNodeInsertTest) {
  // Test Insert
  //  When same key is inserted twice, insert should return nullptr
  // When inserted key and unique_ptr's key does not match, return nullptr
  auto t = TrieNode('a');
  auto child_node = t.InsertChildNode('b', std::make_unique<TrieNode>('b'));
  EXPECT_NE(child_node, nullptr);
  EXPECT_EQ((*child_node)->GetKeyChar(), 'b');

  child_node = t.InsertChildNode('b', std::make_unique<TrieNode>('b'));
  EXPECT_EQ(child_node, nullptr);

  child_node = t.InsertChildNode('d', std::make_unique<TrieNode>('b'));
  EXPECT_EQ(child_node, nullptr);

  child_node = t.InsertChildNode('c', std::make_unique<TrieNode>('c'));
  EXPECT_EQ((*child_node)->GetKeyChar(), 'c');
}

TEST(StarterTest, TrieNodeRemoveTest) {
  auto t = TrieNode('a');
  __attribute__((unused)) auto child_node = t.InsertChildNode('b', std::make_unique<TrieNode>('b'));
  child_node = t.InsertChildNode('c', std::make_unique<TrieNode>('c'));

  t.RemoveChildNode('b');
  EXPECT_EQ(t.HasChild('b'), false);
  EXPECT_EQ(t.HasChildren(), true);
  child_node = t.GetChildNode('b');
  EXPECT_EQ(child_node, nullptr);

  t.RemoveChildNode('c');
  EXPECT_EQ(t.HasChild('c'), false);
  EXPECT_EQ(t.HasChildren(), false);
  child_node = t.GetChildNode('c');
  EXPECT_EQ(child_node, nullptr);
}

TEST(StarterTest, TrieInsertTest) {
  {
    Trie trie;
    trie.Insert<std::string>("abc", "d");
    bool success = true;
    auto val = trie.GetValue<std::string>("abc", &success);
    EXPECT_EQ(success, true);
    EXPECT_EQ(val, "d");
  }

  // Insert empty string key should return false
  {
    Trie trie;
    auto success = trie.Insert<std::string>("", "d");
    EXPECT_EQ(success, false);
    trie.GetValue<std::string>("", &success);
    EXPECT_EQ(success, false);
  }

  // Insert duplicated key should not modify existing value
  {
    Trie trie;
    bool success = trie.Insert<int>("abc", 5);
    EXPECT_EQ(success, true);

    success = trie.Insert<int>("abc", 6);
    EXPECT_EQ(success, false);

    auto val = trie.GetValue<int>("abc", &success);
    EXPECT_EQ(success, true);
    EXPECT_EQ(val, 5);
  }

  // Insert different data types
  {
    Trie trie;
    bool success = trie.Insert<int>("a", 5);
    EXPECT_EQ(success, true);
    success = trie.Insert<std::string>("aa", "val");
    EXPECT_EQ(success, true);

    EXPECT_EQ(trie.GetValue<int>("a", &success), 5);
    EXPECT_EQ(success, true);
    EXPECT_EQ(trie.GetValue<std::string>("aa", &success), "val");
    EXPECT_EQ(success, true);

    trie.GetValue<int>("aaaa", &success);
    EXPECT_EQ(success, false);
  }
}

TEST(StarterTrieTest, RemoveTest) {
  {
    Trie trie;
    bool success = trie.Insert<int>("a", 5);
    EXPECT_EQ(success, true);
    success = trie.Insert<int>("aa", 6);
    EXPECT_EQ(success, true);
    success = trie.Insert<int>("aaa", 7);
    EXPECT_EQ(success, true);

    success = trie.Remove("aaa");
    EXPECT_EQ(success, true);
    trie.GetValue<int>("aaa", &success);
    EXPECT_EQ(success, false);

    success = trie.Insert("aaa", 8);
    EXPECT_EQ(success, true);
    EXPECT_EQ(trie.GetValue<int>("aaa", &success), 8);
    EXPECT_EQ(success, true);

    // Remove non-existant keys should return false
    success = trie.Remove("aaaa");
    EXPECT_EQ(success, false);

    success = trie.Remove("aa");
    EXPECT_EQ(success, true);
    success = trie.Remove("a");
    EXPECT_EQ(success, true);
    success = trie.Remove("aaa");
    EXPECT_EQ(success, true);
  }
}

TEST(StarterTrieTest, ConcurrentTest1) {
  Trie trie;
  constexpr int num_words = 1000;
  constexpr int num_bits = 10;

  std::vector<std::thread> threads;
  threads.reserve(num_words);

  auto insert_task = [&](const std::string &key, int value) {
    bool success = trie.Insert(key, value);
    EXPECT_EQ(success, true);
  };
  for (int i = 0; i < num_words; i++) {
    std::string key = std::bitset<num_bits>(i).to_string();
    threads.emplace_back(std::thread{insert_task, key, i});
  }
  for (int i = 0; i < num_words; i++) {
    threads[i].join();
  }
  threads.clear();

  auto get_task = [&](const std::string &key, int value) {
    bool success = false;
    int tval = trie.GetValue<int>(key, &success);
    EXPECT_EQ(success, true);
    EXPECT_EQ(tval, value);
  };
  for (int i = 0; i < num_words; i++) {
    std::string key = std::bitset<num_bits>(i).to_string();
    threads.emplace_back(std::thread{get_task, key, i});
  }
  for (int i = 0; i < num_words; i++) {
    threads[i].join();
  }
  threads.clear();
}

}  // namespace bustub
