//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// robin_hood_hash_set.h
//
// Identification: src/include/primer/robin_hood_hash_set.h
//
// Copyright (c) 2015-2026, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>  // NOLINT
#include <string>
#include <utility>

namespace bustub {

/** Deterministic hash function for the key types supported by this project. */
template <typename KeyType>
struct RobinHoodHash;

template <>
struct RobinHoodHash<int> {
  auto operator()(int key) const -> size_t { return static_cast<size_t>(key); }
};

template <>
struct RobinHoodHash<int64_t> {
  auto operator()(int64_t key) const -> size_t { return static_cast<size_t>(key); }
};

template <>
struct RobinHoodHash<std::string> {
  auto operator()(const std::string &key) const -> size_t {
    size_t hash = 0;
    for (const unsigned char byte : key) {
      hash = hash * 31 + byte;
    }
    return hash;
  }
};

/** A fixed-capacity, concurrent hash set using Robin Hood open addressing. */
template <typename KeyType, typename Hash = RobinHoodHash<KeyType>, typename KeyEqual = std::equal_to<KeyType>>
class RobinHoodHashSet {
 public:
  explicit RobinHoodHashSet(size_t capacity, Hash hasher = Hash{}, KeyEqual key_equal = KeyEqual{});

  RobinHoodHashSet() = delete;
  RobinHoodHashSet(const RobinHoodHashSet &) = delete;
  auto operator=(const RobinHoodHashSet &) -> RobinHoodHashSet & = delete;

  RobinHoodHashSet(RobinHoodHashSet &&other) noexcept;
  auto operator=(RobinHoodHashSet &&other) noexcept -> RobinHoodHashSet &;

  /** @return true when a key is inserted or an existing equal key is replaced, false when full. */
  auto Insert(const KeyType &key) -> bool;
  /** @return true if key was present and was marked deleted. */
  auto Remove(const KeyType &key) -> bool;
  void Clear();

  auto Contains(const KeyType &key) const -> bool;
  auto GetBucket(const KeyType &key) const -> size_t;
  auto Size() const -> size_t;
  auto Capacity() const -> size_t;
  auto BucketCount() const -> size_t { return Capacity(); }
  auto LoadFactor() const -> double;
  auto MaxProbeDistance() const -> size_t;

 private:
  // TODO(student): Define the bucket representation, synchronization
  // primitives, and metadata needed by your implementation.
};

}  // namespace bustub
