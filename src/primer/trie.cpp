#include "primer/trie.h"
#include <functional>
#include <string_view>
#include <type_traits>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  // throw NotImplementedException("Trie::Get is not implemented.");
  auto root = root_;
  if (!root) {
    return nullptr;
  }
  for (char c : key) {
    if (root && root->children_.count(c)) {
      root = root->children_.at(c);
    } else {
      return nullptr;
    }
  }
  if (root && root->is_value_node_) {
    auto value_node = dynamic_cast<const TrieNodeWithValue<T> *>(root.get());
    if (value_node) {
      return value_node->value_.get();
    }
  }
  return nullptr;
  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {
  // Note that `T` might be a non-copyable type. Always use `std::move` when creating `shared_ptr` on that value.
  // throw NotImplementedException("Trie::Put is not implemented.");
  int n = static_cast<int>(key.size());
  std::function<std::shared_ptr<const TrieNode>(const std::shared_ptr<const TrieNode> &, int)> creat_node =
      [&](const std::shared_ptr<const TrieNode> &root, int u) -> std::shared_ptr<const TrieNode> {
    std::map<char, std::shared_ptr<const TrieNode>> children;
    if (root) {
      children = root->children_;
    }
    if (u == n) {
      return std::make_shared<const TrieNodeWithValue<T>>(children, std::make_shared<T>(std::move(value)));
    }
    children[key[u]] = creat_node(root && root->children_.count(key[u]) ? root->children_.at(key[u]) : nullptr, u + 1);
    return root ? root->CloneWithMap(children) : std::make_shared<const TrieNode>(children);
  };
  return Trie(creat_node(root_, 0));

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}

auto Trie::Remove(std::string_view key) const -> Trie {
  // throw NotImplementedException("Trie::Remove is not implemented.");
  if (!root_) {
    return Trie(root_);
  }
  int n = static_cast<int>(key.size());
  std::function<std::shared_ptr<const TrieNode>(const std::shared_ptr<const TrieNode> &, int)> delete_node =
      [&](const std::shared_ptr<const TrieNode> &root, int u) -> std::shared_ptr<const TrieNode> {
    std::map<char, std::shared_ptr<const TrieNode>> children;
    if (root) {
      children = root->children_;
    }
    if (u == n) {
      return std::make_shared<const TrieNode>(children);
    }
    if (children.count(key[u]) != 0U) {
      children[key[u]] =
          delete_node(root && root->children_.count(key[u]) != 0U ? root->children_.at(key[u]) : root, u + 1);
    }
    return root ? root->CloneWithMap(children) : std::make_shared<const TrieNode>(children);
  };
  return Trie(delete_node(root_, 0));
  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

template auto Trie::Put(std::string_view key, uint32_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint32_t *;

template auto Trie::Put(std::string_view key, uint64_t value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const uint64_t *;

template auto Trie::Put(std::string_view key, std::string value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const std::string *;

// If your solution cannot compile for non-copy tests, you can remove the below lines to get partial score.

using Integer = std::unique_ptr<uint32_t>;

template auto Trie::Put(std::string_view key, Integer value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const Integer *;

template auto Trie::Put(std::string_view key, MoveBlocked value) const -> Trie;
template auto Trie::Get(std::string_view key) const -> const MoveBlocked *;

}  // namespace bustub
