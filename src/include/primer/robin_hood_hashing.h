//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// robin_hood_hashing.h
//
// Identification: src/include/primer/robin_hood_hashing.h
//
// Copyright (c) 2015-2026, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <atomic>
#include <cstddef>
#include <functional>
#include <mutex>  // NOLINT
#include <utility>

namespace bustub {

/** A fixed-capacity, concurrent hash set using Robin Hood open addressing. */
template <typename KeyType, typename Hash = std::hash<KeyType>, typename KeyEqual = std::equal_to<KeyType>>
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
