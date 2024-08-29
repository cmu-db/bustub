#pragma once

#include <bitset>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "murmur3/MurmurHash3.h"

namespace bustub {

template <typename KeyType>
class PrimerHashFunction {
 private:
  /** @brief hash value type */
  using hash_t = uint64_t;

  /** @brief seed constant*/
  static const u_int32_t SEED = 0;

 public:
  auto inline GetHash(KeyType val) {
    /** @brief Intermediate hash. */
    hash_t tmp;

    if (std::is_same<KeyType, std::string>::value) {
      /** @brief Convert into output stream */
      std::ostringstream o_stream;

      o_stream << val;  // output stream

      /** @brief Output stream result. */
      std::string res = o_stream.str();

      tmp = murmur3::MurmurHash3_x64_128(reinterpret_cast<const void *>(res.c_str()), static_cast<int>(res.length()),
                                         SEED);
    } else {
      tmp = murmur3::MurmurHash3_x64_128(reinterpret_cast<const void *>(&val), static_cast<int>(sizeof(KeyType)), SEED);
    }
    hash_t hash = (static_cast<uint64_t>(tmp) << 32) |
                  (static_cast<uint32_t>(murmur3::MurmurHash3_x64_128(reinterpret_cast<const void *>(&tmp),
                                                                      sizeof(typeid(tmp).name()), SEED)));
    return hash;
  }
};
}  // namespace bustub
