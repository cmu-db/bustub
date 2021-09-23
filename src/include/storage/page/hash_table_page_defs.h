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

/** BLOCK_ARRAY_SIZE is the number of (key, value) pairs that can be stored in   * a block page. It is an approximate
 * calculation based on the size of MappingType (which is a std::pair of KeyType and ValueType). For each key/value
 * pair, we need two additional bits for occupied_ and readable_. 4 * PAGE_SIZE / (4 * sizeof (MappingType) + 1) =
 * PAGE_SIZE/(sizeof (MappingType) + 0.25) because 0.25 bytes = 2 bits is the space required to maintain the occupied
 * and readable flags for a key value pair.*/
// #define BLOCK_ARRAY_SIZE (4 * PAGE_SIZE / (4 * sizeof(MappingType) + 1))


/** BUCKET_ARRAY_SIZE is the number of (key, value) pairs that can be stored in a block page.
 * It is an approximate calculation based on the size of MappingType (which is a std::pair of KeyType and ValueType). For each key/value
 * pair, we need two additional bits for occupied_ and readable_. 4 * (PAGE_SIZE - 4) / (4 * sizeof (MappingType) + 1) =
 * (PAGE_SIZE - 4)/(sizeof (MappingType) + 0.25) because 0.25 bytes = 2 bits is the space required to maintain the occupied
 * and readable flags for a key value pair, and we need 4 bytes in the page to store local depth*/
#define BUCKET_ARRAY_SIZE (4 * (PAGE_SIZE - 8) / (4 * sizeof(MappingType) + 1))

/** BUCKET_ARRAY_SIZE is the number of page_ids that can be stored in an extendible hash table directory.
 *  We have 16 bytes of header information, and each page_id_t is 4 bytes.  Hence, we have 
 *  DIRECTORY_ARRAY_SIZE = (4096 bytes per page - 16 bytes in header) / 4 bytes per page_id_t
 */
#define DIRECTORY_ARRAY_SIZE ((PAGE_SIZE - 16) / 4)


// #define HASH_TABLE_BLOCK_TYPE HashTableBlockPage<KeyType, ValueType, KeyComparator>
#define HASH_TABLE_BUCKET_TYPE HashTableBucketPage<KeyType, ValueType, KeyComparator>

#define HASH_BUCKET_INSERT_SUCCESS 0
#define HASH_BUCKET_INSERT_DUPLICATE 1
#define HASH_BUCKET_INSERT_FULL 2

