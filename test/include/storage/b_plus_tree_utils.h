// :bustub-keep-private:
//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree_utils.h
//
// Identification: test/include/storage/b_plus_tree_utils.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <utility>
#include <vector>

#include "buffer/buffer_pool_manager.h"
#include "storage/index/b_plus_tree.h"

namespace bustub {

template <typename KeyType, typename ValueType, typename KeyComparator, ssize_t NumTombs = 0>
bool IsTreeValidImpl(page_id_t node_page_id, BufferPoolManager *bpm, const KeyComparator &comparator,
                     KeyType lower_bound, KeyType upper_bound, bool is_lower_neg_inf, bool is_upper_inf) {
  auto pg = bpm->ReadPage(node_page_id);
  auto page = pg.As<BPlusTreePage>();
  bool is_valid = true;
  if (page->IsLeafPage()) {
    auto leaf = pg.As<BPlusTreeLeafPage<KeyType, ValueType, KeyComparator, NumTombs>>();
    if (leaf->GetSize() == 0) {
      return false;
    }
    for (int i = 0; i < leaf->GetSize(); i++) {
      // Overall ordering validity
      if (!is_lower_neg_inf && comparator(leaf->KeyAt(i), lower_bound) < 0) {
        is_valid = false;
        break;
      }
      if (!is_upper_inf && comparator(leaf->KeyAt(i), upper_bound) >= 0) {
        is_valid = false;
        break;
      }

      // Ordering validity within the page
      if (i - 1 > -1 && comparator(leaf->KeyAt(i - 1), leaf->KeyAt(i)) > 0) {
        is_valid = false;
        break;
      }
    }
  } else {
    auto internal = pg.As<BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator>>();
    if (internal->GetSize() == 0) {
      return false;
    }
    for (int i = 1; i < internal->GetSize(); i++) {
      if (!is_lower_neg_inf && comparator(internal->KeyAt(i), lower_bound) < 0) {
        is_valid = false;
        break;
      }
      if (!is_upper_inf && comparator(internal->KeyAt(i), upper_bound) >= 0) {
        is_valid = false;
        break;
      }
      if (i - 1 > 0 && comparator(internal->KeyAt(i - 1), internal->KeyAt(i)) > 0) {
        is_valid = false;
        break;
      }
      if (!IsTreeValidImpl<KeyType, ValueType, KeyComparator, NumTombs>(
              internal->ValueAt(i - 1), bpm, comparator, lower_bound, internal->KeyAt(i), is_lower_neg_inf, false)) {
        is_valid = false;
        break;
      }
      lower_bound = internal->KeyAt(i);
      is_lower_neg_inf = false;
    }
    is_valid = is_valid && IsTreeValidImpl<KeyType, ValueType, KeyComparator, NumTombs>(
                               internal->ValueAt(internal->GetSize() - 1), bpm, comparator, lower_bound, upper_bound,
                               is_lower_neg_inf, is_upper_inf);
  }
  return is_valid;
}

template <typename KeyType, typename KeyValue, typename KeyComparator, ssize_t NumTombs = 0>
bool IsTreeValid(page_id_t root_page_id, BufferPoolManager *bpm, const KeyComparator &comparator) {
  return IsTreeValidImpl<KeyType, KeyValue, KeyComparator, NumTombs>(root_page_id, bpm, comparator, {}, {}, true, true);
}

template <typename KeyType, typename KeyValue, typename KeyComparator, ssize_t NumTombs = 0>
bool TreeValuesMatch(BPlusTree<KeyType, KeyValue, KeyComparator, NumTombs> &tree, std::vector<int64_t> &inserted,
                     std::vector<int64_t> &deleted) {
  std::vector<KeyValue> rids;
  KeyType index_key;
  for (auto &key : inserted) {
    rids.clear();
    index_key.SetFromInteger(key);
    tree.GetValue(index_key, &rids);
    if (rids.size() != 1) {
      return false;
    }
  }
  for (auto &key : deleted) {
    rids.clear();
    index_key.SetFromInteger(key);
    tree.GetValue(index_key, &rids);
    if (!rids.empty()) {
      return false;
    }
  }
  return true;
}

template <typename KeyType, typename ValueType, typename KeyComparator, ssize_t NumTombs = 0>
page_id_t GetLeftMostLeafPageId(page_id_t root_page_id, BufferPoolManager *bpm) {
  auto pg = bpm->ReadPage(root_page_id);
  auto page = pg.As<BPlusTreePage>();
  if (page->IsLeafPage()) {
    return root_page_id;
  }
  auto internal = pg.As<BPlusTreeInternalPage<KeyType, page_id_t, KeyComparator>>();
  if (internal->GetSize() == 0) {
    return INVALID_PAGE_ID;
  }
  auto leftmost_child_id = internal->ValueAt(0);
  pg.Drop();
  return GetLeftMostLeafPageId<KeyType, ValueType, KeyComparator, NumTombs>(leftmost_child_id, bpm);
}

FULL_INDEX_TEMPLATE_ARGUMENTS_DEFN
class IndexLeaves {
 public:
  BufferPoolManager *buffer_pool_manager_;
  std::optional<ReadPageGuard> guard_;

  IndexLeaves(page_id_t root_page, BufferPoolManager *buffer_pool_manager) : buffer_pool_manager_(buffer_pool_manager) {
    auto pid = GetLeftMostLeafPageId<KeyType, ValueType, KeyComparator>(root_page, buffer_pool_manager);
    guard_ = buffer_pool_manager->ReadPage(pid);
  }

  ~IndexLeaves() = default;

  IndexLeaves(IndexLeaves &&) noexcept = default;
  auto operator=(IndexLeaves &&) noexcept -> IndexLeaves & = default;

  auto operator*() -> const B_PLUS_TREE_LEAF_PAGE_TYPE * {
    BUSTUB_ASSERT(guard_.has_value(), "invalid iterator");
    return guard_.value().template As<B_PLUS_TREE_LEAF_PAGE_TYPE>();
  }

  auto operator++() -> IndexLeaves<KeyType, ValueType, KeyComparator, NumTombs> & {
    BUSTUB_ASSERT(guard_.has_value(), "invalid iterator");
    auto leaf = guard_.value().template As<B_PLUS_TREE_LEAF_PAGE_TYPE>();
    if (leaf->GetNextPageId() != INVALID_PAGE_ID) {
      auto guard = buffer_pool_manager_->ReadPage(leaf->GetNextPageId());
      guard_ = std::move(guard);
    } else {
      guard_ = std::nullopt;
    }
    return *this;
  }

  DISALLOW_COPY(IndexLeaves);

  auto Valid() -> bool { return guard_.has_value(); }
};

FULL_INDEX_TEMPLATE_ARGUMENTS
auto GetNumLeaves(BPlusTree<KeyType, ValueType, KeyComparator, NumTombs> &tree, BufferPoolManager *bpm) -> size_t {
  auto leaf = IndexLeaves<KeyType, ValueType, KeyComparator, NumTombs>(tree.GetRootPageId(), bpm);
  size_t count = 0;
  while (leaf.Valid()) {
    count++;
    ++leaf;
  }
  return count;
}

}  // namespace bustub
