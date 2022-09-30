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
 * key/value pair, we need two additional bits for occupied_ and readable_. 4 * BUSTUB_PAGE_SIZE / (4 * sizeof
 * (MappingType) + 1) = BUSTUB_PAGE_SIZE/(sizeof (MappingType) + 0.25) because 0.25 bytes = 2 bits is the space required
 * to maintain the occupied and readable flags for a key value pair.
 */
#define BLOCK_ARRAY_SIZE (4 * BUSTUB_PAGE_SIZE / (4 * sizeof(MappingType) + 1))

/**
 * Extendible Hashing Definitions
 */
#define HASH_TABLE_BUCKET_TYPE HashTableBucketPage<KeyType, ValueType, KeyComparator>

/**
 * BUCKET_ARRAY_SIZE is the number of (key, value) pairs that can be stored in an extendible hash index bucket page.
 * The computation is the same as the above BLOCK_ARRAY_SIZE, but blocks and buckets have different implementations
 * of search, insertion, removal, and helper methods.
 */
#define BUCKET_ARRAY_SIZE (4 * BUSTUB_PAGE_SIZE / (4 * sizeof(MappingType) + 1))

/**
 * DIRECTORY_ARRAY_SIZE is the number of page_ids that can fit in the directory page of an extendible hash index.
 * This is 512 because the directory array must grow in powers of 2, and 1024 page_ids leaves zero room for
 * storage of the other member variables: page_id_, lsn_, global_depth_, and the array local_depths_.
 * Extending the directory implementation to span multiple pages would be a meaningful improvement to the
 * implementation.
 */
#define DIRECTORY_ARRAY_SIZE 512
