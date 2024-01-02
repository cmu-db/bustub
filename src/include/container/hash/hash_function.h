//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_function.h
//
// Identification: src/include/container/hash/hash_function.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>

#include "murmur3/MurmurHash3.h"

namespace bustub {

template <typename KeyType>
class HashFunction {
 public:
  /**
   * @param key the key to be hashed
   * @return the hashed value
   */
  virtual auto GetHash(KeyType key) const -> uint64_t {
    uint64_t hash[2];
    murmur3::MurmurHash3_x64_128(reinterpret_cast<const void *>(&key), static_cast<int>(sizeof(KeyType)), 0,
                                 reinterpret_cast<void *>(&hash));
    return hash[0];
  }
};

}  // namespace bustub
