//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_page_defs.h
//
// Identification: src/include/storage/page/hash_table_page_defs.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#define MappingType std::pair<KeyType, ValueType>

#define BLOCK_ARRAY_SIZE (4 * PAGE_SIZE / (4 * sizeof(MappingType) + 1))

#define HASH_TABLE_BLOCK_TYPE HashTableBlockPage<KeyType, ValueType, KeyComparator>
