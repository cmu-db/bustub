//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// linear_probe_hash_table.cpp
//
// Identification: src/container/hash/linear_probe_hash_table.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <string>
#include <vector>

#include "common/rid.h"
#include "container/hash/linear_probe_hash_table.h"

namespace bustub {

INDEX_TEMPLATE_ARGUMENTS
HASH_TABLE_TYPE::LinearProbeHashTable(const std::string &name, BufferPoolManager *buffer_pool_manager,
                                      const KeyComparator &comparator, size_t num_buckets)
    : buffer_pool_manager_(buffer_pool_manager), comparator_(comparator) {}

INDEX_TEMPLATE_ARGUMENTS
bool HASH_TABLE_TYPE::GetValue(Transaction *transaction, const KeyType &key, std::vector<ValueType> *result) {
  return false;
}

INDEX_TEMPLATE_ARGUMENTS
void HASH_TABLE_TYPE::Insert(Transaction *transaction, const KeyType &key, const ValueType &value) {}

INDEX_TEMPLATE_ARGUMENTS
void HASH_TABLE_TYPE::Remove(Transaction *transaction, const KeyType &key) {}

INDEX_TEMPLATE_ARGUMENTS
void HASH_TABLE_TYPE::Resize(size_t initial_size) {}

template class LinearProbeHashTable<GenericKey<4>, RID, GenericComparator<4>>;
template class LinearProbeHashTable<GenericKey<8>, RID, GenericComparator<8>>;
template class LinearProbeHashTable<GenericKey<16>, RID, GenericComparator<16>>;
template class LinearProbeHashTable<GenericKey<32>, RID, GenericComparator<32>>;
template class LinearProbeHashTable<GenericKey<64>, RID, GenericComparator<64>>;
template class LinearProbeHashTable<int, int, IntComparator>;

}  // namespace bustub
