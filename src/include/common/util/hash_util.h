//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hash_util.h
//
// Identification: src/include/common/util/hash_util.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>

#include "common/macros.h"
#include "type/value.h"

namespace bustub {

using hash_t = std::size_t;

class HashUtil {
 private:
  static const hash_t PRIME_FACTOR = 10000019;

 public:
  static inline hash_t HashBytes(const char *bytes, size_t length) {
    // https://github.com/greenplum-db/gpos/blob/b53c1acd6285de94044ff91fbee91589543feba1/libgpos/src/utils.cpp#L126
    hash_t hash = length;
    for (size_t i = 0; i < length; ++i) {
      hash = ((hash << 5) ^ (hash >> 27)) ^ bytes[i];
    }
    return hash;
  }

  static inline hash_t CombineHashes(hash_t l, hash_t r) {
    hash_t both[2] = {};
    both[0] = l;
    both[1] = r;
    return HashBytes(reinterpret_cast<char *>(both), sizeof(hash_t) * 2);
  }

  static inline hash_t SumHashes(hash_t l, hash_t r) { return (l % PRIME_FACTOR + r % PRIME_FACTOR) % PRIME_FACTOR; }

  template <typename T>
  static inline hash_t Hash(const T *ptr) {
    return HashBytes(reinterpret_cast<const char *>(ptr), sizeof(T));
  }

  template <typename T>
  static inline hash_t HashPtr(const T *ptr) {
    return HashBytes(reinterpret_cast<const char *>(&ptr), sizeof(void *));
  }

  /** @return the hash of the value */
  static inline hash_t HashValue(const Value *val) {
    switch (val->GetTypeId()) {
      case TypeId::TINYINT: {
        auto raw = static_cast<int64_t>(val->GetAs<int8_t>());
        return Hash<int64_t>(&raw);
      }
      case TypeId::SMALLINT: {
        auto raw = static_cast<int64_t>(val->GetAs<int16_t>());
        return Hash<int64_t>(&raw);
      }
      case TypeId::INTEGER: {
        auto raw = static_cast<int64_t>(val->GetAs<int32_t>());
        return Hash<int64_t>(&raw);
      }
      case TypeId::BIGINT: {
        auto raw = static_cast<int64_t>(val->GetAs<int64_t>());
        return Hash<int64_t>(&raw);
      }
      case TypeId::BOOLEAN: {
        auto raw = val->GetAs<bool>();
        return Hash<bool>(&raw);
      }
      case TypeId::DECIMAL: {
        auto raw = val->GetAs<double>();
        return Hash<double>(&raw);
      }
      case TypeId::VARCHAR: {
        auto raw = val->GetData();
        auto len = val->GetLength();
        return HashBytes(raw, len);
      }
      case TypeId::TIMESTAMP: {
        auto raw = val->GetAs<uint64_t>();
        return Hash<uint64_t>(&raw);
      }
      default: {
        BUSTUB_ASSERT(false, "Unsupported type.");
      }
    }
  }
};

}  // namespace bustub
