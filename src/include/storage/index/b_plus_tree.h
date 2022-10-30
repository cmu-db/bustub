//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/include/index/b_plus_tree.h
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
/**
 * b_plus_tree.h
 *
 * Implementation of simple b+ tree data structure where internal pages direct
 * the search and leaf pages contain actual data.
 * (1) We only support unique key
 * (2) support insert & remove
 * (3) The structure should shrink and grow dynamically
 * (4) Implement index iterator for range scan
 */
#pragma once

#include <algorithm>
#include <map>
#include <mutex>  // NOLINT
#include <queue>
#include <string>
#include <vector>

#include "common/config.h"
#include "common/exception.h"
#include "concurrency/transaction.h"
#include "storage/index/index_iterator.h"
#include "storage/index/stl_comparator_wrapper.h"
#include "storage/page/b_plus_tree_internal_page.h"
#include "storage/page/b_plus_tree_leaf_page.h"

namespace bustub {

#define BPLUSTREE_TYPE BPlusTree<KeyType, ValueType, KeyComparator>
// Main class providing the API for the Interactive B+ Tree.
INDEX_TEMPLATE_ARGUMENTS
class BPlusTree {
  using InternalPage = BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator>;
  using LeafPage = BPlusTreeLeafPage<KeyType, ValueType, KeyComparator>;

 public:
  explicit BPlusTree(std::string name, BufferPoolManager *buffer_pool_manager, const KeyComparator &comparator,
                     int leaf_max_size = LEAF_PAGE_SIZE, int internal_max_size = INTERNAL_PAGE_SIZE);

  // Returns true if this B+ tree has no keys and values.
  auto IsEmpty() const -> bool;

  // Insert a key-value pair into this B+ tree.
  auto Insert(const KeyType &key, const ValueType &value, Transaction *transaction = nullptr) -> bool;

  // Remove a key and its value from this B+ tree.
  void Remove(const KeyType &key, Transaction *transaction);

  // return the value associated with a given key
  auto GetValue(const KeyType &key, std::vector<ValueType> *result, Transaction *transaction = nullptr) -> bool;

  // index iterator
  auto Begin() -> INDEXITERATOR_TYPE;

  auto End() -> INDEXITERATOR_TYPE;

  auto Begin(const KeyType &key) -> INDEXITERATOR_TYPE;

  // read data from file and insert one by one
  void InsertFromFile(const std::string &file_name, Transaction *transaction = nullptr);

  // read data from file and remove one by one
  void RemoveFromFile(const std::string &file_name, Transaction *transaction = nullptr);

  void Print(BufferPoolManager *bpm) { throw bustub::NotImplementedException("not implemented"); }

  // draw the B+ tree
  void Draw(BufferPoolManager *bpm, const std::string &outf) {
    throw bustub::NotImplementedException("not implemented");
  }

  /* Debug Routines for FREE!! */
  void ToGraph(BPlusTreePage *page, BufferPoolManager *bpm, std::ofstream &out) const {
    throw bustub::NotImplementedException("not implemented");
  }

  auto FindLeafPage(const KeyType &key, bool leftMost = false) -> Page * {
    throw bustub::NotImplementedException("not implemented");
  }

  auto GetRootPageId() -> page_id_t { return INVALID_PAGE_ID; }

 private:
  std::mutex lock_;
  std::string index_name_;
  BufferPoolManager *buffer_pool_manager_;
  StlComparatorWrapper<KeyType, KeyComparator> comparator_;
  std::map<KeyType, ValueType, StlComparatorWrapper<KeyType, KeyComparator>> data_;
  int leaf_max_size_;
  int internal_max_size_;
};

}  // namespace bustub
