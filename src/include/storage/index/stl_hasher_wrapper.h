#pragma once

#include "container/hash/hash_function.h"

namespace bustub {

template <typename K>
class StlHasherWrapper {
 public:
  explicit StlHasherWrapper(const HashFunction<K> &hash_fn) : hash_fn_{hash_fn} {}

  inline auto operator()(const K &key) const -> std::size_t { return hash_fn_.GetHash(key); }

  HashFunction<K> hash_fn_;
};

}  // namespace bustub
