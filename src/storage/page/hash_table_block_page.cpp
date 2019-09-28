//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_block_page.cpp
//
// Identification: src/storage/page/hash_table_block_page.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/page/hash_table_block_page.h"
#include "storage/index/generic_key.h"

namespace bustub {

INDEX_TEMPLATE_ARGUMENTS
KeyType HASH_TABLE_BLOCK_TYPE::KeyAt(size_t bucket_ind) const { return {}; }

INDEX_TEMPLATE_ARGUMENTS
ValueType HASH_TABLE_BLOCK_TYPE::ValueAt(size_t bucket_ind) const { return {}; }

INDEX_TEMPLATE_ARGUMENTS
bool HASH_TABLE_BLOCK_TYPE::Insert(size_t bucket_ind, const KeyType &key, const ValueType &value) { return false; }

INDEX_TEMPLATE_ARGUMENTS
void HASH_TABLE_BLOCK_TYPE::Remove(size_t bucket_ind) {}

INDEX_TEMPLATE_ARGUMENTS
bool HASH_TABLE_BLOCK_TYPE::IsOccupied(size_t bucket_ind) { return false; }

INDEX_TEMPLATE_ARGUMENTS
bool HASH_TABLE_BLOCK_TYPE::IsReadable(size_t bucket_ind) { return false; }

template class HashTableBlockPage<int, int, IntComparator>;
template class HashTableBlockPage<GenericKey<4>, RID, GenericComparator<4>>;
template class HashTableBlockPage<GenericKey<8>, RID, GenericComparator<8>>;
template class HashTableBlockPage<GenericKey<16>, RID, GenericComparator<16>>;
template class HashTableBlockPage<GenericKey<32>, RID, GenericComparator<32>>;
template class HashTableBlockPage<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
