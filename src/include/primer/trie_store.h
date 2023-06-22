#pragma once

#include <optional>
#include <shared_mutex>
#include <utility>

#include "primer/trie.h"

namespace bustub {

// This class is used to guard the value returned by the trie. It holds a reference to the root so
// that the reference to the value will not be invalidated.
template <class T>
class ValueGuard {
 public:
  ValueGuard(Trie root, const T &value) : root_(std::move(root)), value_(value) {}
  auto operator*() const -> const T & { return value_; }

 private:
  Trie root_;
  const T &value_;
};

// This class is a thread-safe wrapper around the Trie class. It provides a simple interface for
// accessing the trie. It should allow concurrent reads and a single write operation at the same
// time.
class TrieStore {
 public:
  // This function returns a ValueGuard object that holds a reference to the value in the trie. If
  // the key does not exist in the trie, it will return std::nullopt.
  template <class T>
  auto Get(std::string_view key) -> std::optional<ValueGuard<T>>;

  // This function will insert the key-value pair into the trie. If the key already exists in the
  // trie, it will overwrite the value.
  template <class T>
  void Put(std::string_view key, T value);

  // This function will remove the key-value pair from the trie.
  void Remove(std::string_view key);

 private:
  // This mutex protects the root. Every time you want to access the trie root or modify it, you
  // will need to take this lock.
  std::mutex root_lock_;

  // This mutex sequences all writes operations and allows only one write operation at a time.
  std::mutex write_lock_;

  // Stores the current root for the trie.
  Trie root_;
};

}  // namespace bustub
