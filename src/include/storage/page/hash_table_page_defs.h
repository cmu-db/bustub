//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_page_defs.h
//
// Identification: src/include/storage/page/hash_table_page_defs.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#define MappingType std::pair<KeyType, ValueType>

/**
 * Linear Probe Hashing Definitions
 */
#define HASH_TABLE_BLOCK_TYPE HashTableBlockPage<KeyType, ValueType, KeyComparator>

/**
 * BLOCK_ARRAY_SIZE is the number of (key, value) pairs that can be stored in a linear probe hash block page. It is an
 * approximate calculation based on the size of MappingType (which is a std::pair of KeyType and ValueType). For each
 * key/value pair, we need two additional bits for occupied_ and readable_. 4 * PAGE_SIZE / (4 * sizeof (MappingType) +
 * 1) = PAGE_SIZE/(sizeof (MappingType) + 0.25) because 0.25 bytes = 2 bits is the space required to maintain the
 * occupied and readable flags for a key value pair.
 */
#define BLOCK_ARRAY_SIZE (4 * PAGE_SIZE / (4 * sizeof(MappingType) + 1))

/**
 * Extendible Hashing Definitions
 */
#define HASH_TABLE_BUCKET_TYPE HashTableBucketPage<KeyType, ValueType, KeyComparator>
#define DIRECTORY_ARRAY_SIZE 512

/**
 * BUCKET_ARRAY_SIZE is the number of (key, value) pairs that can be stored in an extendible hashing bucket page.
 * It is an approximate calculation based on the size of MappingType (which is a std::pair of KeyType and ValueType).
 * For each key/value pair, we need two additional bits for occupied_ and readable_. 4 * (PAGE_SIZE - 4) / (4 * sizeof
 * (MappingType) + 1) = (PAGE_SIZE - 4)/(sizeof (MappingType) + 0.25) because 0.25 bytes = 2 bits is the space required
 * to maintain the occupied and readable flags for a key value pair.
 */
#define BUCKET_ARRAY_SIZE (4 * PAGE_SIZE / (4 * sizeof(MappingType) + 1))
