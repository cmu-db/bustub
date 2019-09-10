//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_table_block_page.h
//
// Identification: src/include/storage/page/hash_table_block_page.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <utility>
#include <vector>

#include "common/config.h"
#include "storage/index/int_comparator.h"
#include "storage/page/hash_table_page_defs.h"

namespace bustub {
/**
 * Store indexed key and and value together within block page. Supports
 * non-unique keys.
 */
INDEX_TEMPLATE_ARGUMENTS
class HashTableBlockPage {};

}  // namespace bustub
