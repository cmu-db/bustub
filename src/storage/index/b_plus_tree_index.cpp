//===----------------------------------------------------------------------===//
//
//                         CMU-DB Project (15-445/645)
//                         ***DO NO SHARE PUBLICLY***
//
// Identification: src/index/b_plus_tree_index.cpp
//
// Copyright (c) 2018, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/index/b_plus_tree_index.h"

namespace bustub {
/*
 * Constructor
 */
INDEX_TEMPLATE_ARGUMENTS
BPlusTreeIndex<KeyType, ValueType, KeyComparator>::BPlusTreeIndex(std::unique_ptr<IndexMetadata> &&metadata,
                                                                  BufferPoolManager *buffer_pool_manager)
    : Index(std::move(metadata)), comparator_(GetMetadata()->GetKeySchema()) {
  page_id_t header_page_id;
  buffer_pool_manager->NewPage(&header_page_id);
  container_ = std::make_shared<BPlusTree<KeyType, ValueType, KeyComparator>>(GetMetadata()->GetName(), header_page_id,
                                                                              buffer_pool_manager, comparator_);
}

INDEX_TEMPLATE_ARGUMENTS
auto BPlusTreeIndex<KeyType, ValueType, KeyComparator>::InsertEntry(const Tuple &key, RID rid, Transaction *transaction)
    -> bool {
  // construct insert index key
  KeyType index_key;
  index_key.SetFromKey(key);

  return container_->Insert(index_key, rid, transaction);
}

INDEX_TEMPLATE_ARGUMENTS
void BPlusTreeIndex<KeyType, ValueType, KeyComparator>::DeleteEntry(const Tuple &key, RID rid,
                                                                    Transaction *transaction) {
  // construct delete index key
  KeyType index_key;
  index_key.SetFromKey(key);

  container_->Remove(index_key, transaction);
}

INDEX_TEMPLATE_ARGUMENTS
void BPlusTreeIndex<KeyType, ValueType, KeyComparator>::ScanKey(const Tuple &key, std::vector<RID> *result,
                                                                Transaction *transaction) {
  // construct scan index key
  KeyType index_key;
  index_key.SetFromKey(key);

  container_->GetValue(index_key, result, transaction);
}

INDEX_TEMPLATE_ARGUMENTS
auto BPlusTreeIndex<KeyType, ValueType, KeyComparator>::GetBeginIterator() -> INDEXITERATOR_TYPE {
  return container_->Begin();
}

INDEX_TEMPLATE_ARGUMENTS
auto BPlusTreeIndex<KeyType, ValueType, KeyComparator>::GetBeginIterator(const KeyType &key) -> INDEXITERATOR_TYPE {
  return container_->Begin(key);
}

INDEX_TEMPLATE_ARGUMENTS
auto BPlusTreeIndex<KeyType, ValueType, KeyComparator>::GetEndIterator() -> INDEXITERATOR_TYPE {
  return container_->End();
}

template class BPlusTreeIndex<GenericKey<4>, RID, GenericComparator<4>>;
template class BPlusTreeIndex<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTreeIndex<GenericKey<16>, RID, GenericComparator<16>>;
template class BPlusTreeIndex<GenericKey<32>, RID, GenericComparator<32>>;
template class BPlusTreeIndex<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
