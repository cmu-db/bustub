#include "primer/trie_store.h"
#include "common/exception.h"

namespace bustub {

template <class T>
auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<T>> {
  // Pseudo-code:
  // (1) Take the root lock, get the root, and release the root lock. Don't lookup the value in the
  //     trie while holding the root lock.
  // (2) Lookup the value in the trie.
  // (3) If the value is found, return a ValueGuard object that holds a reference to the value and the
  //     root. Otherwise, return std::nullopt.
  //throw NotImplementedException("TrieStore::Get is not implemented.");
  std::unique_lock<std::mutex> rootLock(root_lock_);  // (1) Take the root lock

  Trie currentRoot = root_;  // Copy the current root
  rootLock.unlock();  // Release the root lock

  // (2) Lookup the value in the trie (assuming you have a function for this)
  auto value = currentRoot.Get<T>(key);

  if (value) {
    std::unique_lock<std::mutex> writeLock(write_lock_);  // Lock for concurrent writes

    return std::make_optional<ValueGuard<T>>(std::move(currentRoot), *value);
  }

  return std::nullopt;  // Value not found

}

template <class T>
void TrieStore::Put(std::string_view key, T value) {
  // You will need to ensure there is only one writer at a time. Think of how you can achieve this.
  // The logic should be somehow similar to `TrieStore::Get`.
  //throw NotImplementedException("TrieStore::Put is not implemented.");
  std::unique_lock<std::mutex> writeLock(write_lock_);
  std::unique_lock<std::mutex> rootLock(root_lock_);  // Take the root lock
  auto currentRoot = root_;  // Copy the current root
  rootLock.unlock();  // Release the root lock

  // Lock for concurrent writes


  // Update the copy of the trie root
  root_ =  currentRoot.Put<T>(key, std::move(value));

  // Update the actual root with the modified cop
  //= currentRoot;  // Update the root with the modified copy
  writeLock.unlock();  // Release the root lxock

}

void TrieStore::Remove(std::string_view key) {
  // You will need to ensure there is only one writer at a time. Think of how you can achieve this.
  // The logic should be somehow similar to `TrieStore::Get`.
  //throw NotImplementedException("TrieStore::Remove is not implemented.");
  std::unique_lock<std::mutex> writeLock(write_lock_);
  std::unique_lock<std::mutex> rootLock(root_lock_);  // Take the root lock
  auto currentRoot = root_;  // Copy the current root
  rootLock.unlock();  // Release the root lock

  // Lock for concurrent writes


  // Update the copy of the trie root
  root_ =  currentRoot.Remove(key);

  // Update the actual root with the modified cop
  //= currentRoot;  // Update the root with the modified copy
  writeLock.unlock();  // Release the root lxock

}

// Below are explicit instantiation of template functions.

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<uint32_t>>;
template void TrieStore::Put(std::string_view key, uint32_t value);

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<std::string>>;
template void TrieStore::Put(std::string_view key, std::string value);

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<Integer>>;
template void TrieStore::Put(std::string_view key, Integer value);

template auto TrieStore::Get(std::string_view key) -> std::optional<ValueGuard<MoveBlocked>>;
template void TrieStore::Put(std::string_view key, MoveBlocked value);

}  // namespace bustub
