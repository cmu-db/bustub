//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// robin_hood_hash_set.cpp
//
// Identification: src/primer/robin_hood_hash_set.cpp
//
// Copyright (c) 2015-2026, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/robin_hood_hash_set.h"

#include <cstdint>
#include <string>

namespace bustub {

template <typename KeyType, typename Hash, typename KeyEqual>
RobinHoodHashSet<KeyType, Hash, KeyEqual>::RobinHoodHashSet(size_t capacity, Hash hasher, KeyEqual key_equal) {
  /** @TODO(student) Initialize the fixed-capacity bucket array and store the supplied functors. */
  static_cast<void>(capacity);
  static_cast<void>(hasher);
  static_cast<void>(key_equal);
}

template <typename KeyType, typename Hash, typename KeyEqual>
RobinHoodHashSet<KeyType, Hash, KeyEqual>::RobinHoodHashSet(RobinHoodHashSet &&other) noexcept {
  /** @TODO(student) Move the table's storage, metadata, functors, and synchronization state safely. */
  static_cast<void>(other);
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::operator=(RobinHoodHashSet &&other) noexcept -> RobinHoodHashSet & {
  /** @TODO(student) Implement move assignment, including self-assignment protection. */
  static_cast<void>(other);
  return *this;
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::Insert(const KeyType &key) -> bool {
  /** @TODO(student) Insert with linear probing, Robin Hood displacement, and safe concurrent access. */
  static_cast<void>(key);
  return false;
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::Remove(const KeyType &key) -> bool {
  /** @TODO(student) Mark an occupied matching slot as a tombstone without breaking concurrent operations. */
  static_cast<void>(key);
  return false;
}

template <typename KeyType, typename Hash, typename KeyEqual>
void RobinHoodHashSet<KeyType, Hash, KeyEqual>::Clear() {
  /** @TODO(student) Remove occupied entries and tombstones while preserving capacity. */
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::Contains(const KeyType &key) const -> bool {
  /** @TODO(student) Probe through tombstones and stop only where correctness permits. */
  static_cast<void>(key);
  return false;
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::GetBucket(const KeyType &key) const -> size_t {
  /** @TODO(student) Return the physical bucket holding key, or BucketCount() when absent. */
  static_cast<void>(key);
  return 0;
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::Size() const -> size_t {
  /** @TODO(student) Return the number of occupied entries safely. */
  return 0;
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::Capacity() const -> size_t {
  /** @TODO(student) Return the fixed capacity safely. */
  return 0;
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::LoadFactor() const -> double {
  /** @TODO(student) Return Size() / Capacity(), including moved-from handling. */
  return 0.0;
}

template <typename KeyType, typename Hash, typename KeyEqual>
auto RobinHoodHashSet<KeyType, Hash, KeyEqual>::MaxProbeDistance() const -> size_t {
  /** @TODO(student) Return the greatest probe distance among live entries. */
  return 0;
}

template class RobinHoodHashSet<int>;
template class RobinHoodHashSet<int64_t>;
template class RobinHoodHashSet<std::string>;

}  // namespace bustub
