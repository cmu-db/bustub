//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// stl_unordered.h
//
// Identification: src/include/storage/index/stl_unordered.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <map>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/rid.h"
#include "container/hash/hash_function.h"
#include "storage/index/b_plus_tree.h"
#include "storage/index/index.h"
#include "storage/index/stl_comparator_wrapper.h"
#include "storage/index/stl_equal_wrapper.h"
#include "storage/index/stl_hasher_wrapper.h"

namespace bustub {

template <typename KT, typename VT, typename Cmp>
class STLUnorderedIndex : public Index {
 public:
  STLUnorderedIndex(std::unique_ptr<IndexMetadata> &&metadata, BufferPoolManager *buffer_pool_manager,
                    const HashFunction<KT> &hash_fn)
      : Index(std::move(metadata)),
        comparator_(StlComparatorWrapper<KT, Cmp>(Cmp(metadata_->GetKeySchema()))),
        hash_fn_(StlHasherWrapper<KT>(hash_fn)),
        eq_(StlEqualWrapper<KT, Cmp>(Cmp(metadata_->GetKeySchema()))),
        data_(0, hash_fn_, eq_) {}

  auto InsertEntry(const Tuple &key, VT rid, Transaction *transaction) -> bool override {
    KT index_key;
    index_key.SetFromKey(key);
    std::scoped_lock<std::mutex> lck(lock_);
    if (data_.find(index_key) != data_.end()) {
      return false;
    }
    data_.emplace(index_key, rid);
    return true;
  }

  void DeleteEntry(const Tuple &key, VT rid, Transaction *transaction) override {
    KT index_key;
    index_key.SetFromKey(key);
    std::scoped_lock<std::mutex> lck(lock_);
    if (auto it = data_.find(index_key); it != data_.end()) {
      data_.erase(it);
      return;
    }
  }

  void ScanKey(const Tuple &key, std::vector<RID> *result, Transaction *transaction) override {
    KT index_key;
    index_key.SetFromKey(key);
    std::scoped_lock<std::mutex> lck(lock_);
    if (auto it = data_.find(index_key); it != data_.end()) {
      *result = std::vector{it->second};
      return;
    }
    *result = {};
  }

 protected:
  std::mutex lock_;
  StlComparatorWrapper<KT, Cmp> comparator_;
  StlHasherWrapper<KT> hash_fn_;
  StlEqualWrapper<KT, Cmp> eq_;
  std::unordered_map<KT, VT, StlHasherWrapper<KT>, StlEqualWrapper<KT, Cmp>> data_;
};

using STLUnorderedIndexForTwoIntegerColumn = STLUnorderedIndex<GenericKey<8>, RID, GenericComparator<8>>;

}  // namespace bustub
