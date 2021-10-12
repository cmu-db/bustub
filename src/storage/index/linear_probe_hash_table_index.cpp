#include <vector>

#include "storage/index/linear_probe_hash_table_index.h"

namespace bustub {
/*
 * Constructor
 */
template <typename KeyType, typename ValueType, typename KeyComparator>
HASH_TABLE_INDEX_TYPE::LinearProbeHashTableIndex(std::unique_ptr<IndexMetadata> &&metadata,
                                                 BufferPoolManager *buffer_pool_manager, size_t num_buckets,
                                                 const HashFunction<KeyType> &hash_fn)
    : Index(std::move(metadata)),
      comparator_(GetMetadata()->GetKeySchema()),
      container_(GetMetadata()->GetName(), buffer_pool_manager, comparator_, num_buckets, hash_fn) {}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_INDEX_TYPE::InsertEntry(const Tuple &key, RID rid, Transaction *transaction) {
  // construct insert index key
  KeyType index_key;
  index_key.SetFromKey(key);

  container_.Insert(transaction, index_key, rid);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_INDEX_TYPE::DeleteEntry(const Tuple &key, RID rid, Transaction *transaction) {
  // construct delete index key
  KeyType index_key;
  index_key.SetFromKey(key);

  container_.Remove(transaction, index_key, rid);
}

template <typename KeyType, typename ValueType, typename KeyComparator>
void HASH_TABLE_INDEX_TYPE::ScanKey(const Tuple &key, std::vector<RID> *result, Transaction *transaction) {
  // construct scan index key
  KeyType index_key;
  index_key.SetFromKey(key);

  container_.GetValue(transaction, index_key, result);
}
template class LinearProbeHashTableIndex<GenericKey<4>, RID, GenericComparator<4>>;
template class LinearProbeHashTableIndex<GenericKey<8>, RID, GenericComparator<8>>;
template class LinearProbeHashTableIndex<GenericKey<16>, RID, GenericComparator<16>>;
template class LinearProbeHashTableIndex<GenericKey<32>, RID, GenericComparator<32>>;
template class LinearProbeHashTableIndex<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
