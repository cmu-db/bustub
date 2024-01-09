#include "primer/trie.h"
#include <bits/c++config.h>
#include <stack>
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  auto explorer = GetRoot();
  if (explorer == nullptr) {
    return nullptr;
  }

  for (char key_char : key) {
    explorer = explorer->FindChildrenAt(key_char);
    if (explorer == nullptr) {
      return nullptr;
    }
  }
  if (auto target_node = std::dynamic_pointer_cast<const TrieNodeWithValue<T>>(explorer)) {
    return target_node->value_.get();
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
  std::shared_ptr<TrieNode> new_root(nullptr);
  if (root_ != nullptr) {
    new_root = std::shared_ptr<TrieNode>(root_->Clone());
  } else {
    new_root = std::make_shared<TrieNode>();
  }

  std::shared_ptr<T> value_ptr = std::make_shared<T>(std::move(value));
  if (key.empty()) {
    new_root = std::make_shared<TrieNodeWithValue<T>>(new_root->children_, value_ptr);
    return Trie(new_root);
  }

  auto explorer = new_root;
  for (decltype(key.size()) i = 0; i < key.size(); i++) {
    char key_char = key[i];
    std::shared_ptr<TrieNode> new_child(nullptr);
    if (auto child_node = explorer->FindChildrenAt(key_char)) {
      if (i != key.size() - 1) {
        new_child = std::shared_ptr(child_node->Clone());
      } else {
        new_child = std::make_shared<TrieNodeWithValue<T>>(child_node->children_, value_ptr);
      }
    } else {
      if (i != key.size() - 1) {
        new_child = std::make_shared<TrieNode>();
      } else {
        new_child = std::make_shared<TrieNodeWithValue<T>>(value_ptr);
      }
    }
    explorer->children_[key_char] = new_child;
    explorer = new_child;
  }
  return Trie(new_root);
  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.
}

auto Trie::Remove(std::string_view key) const -> Trie {
  std::stack<std::shared_ptr<const TrieNode>> stk;
  auto explorer = GetRoot();
  for (char key_char : key) {
    stk.push(explorer);
    explorer = explorer->FindChildrenAt(key_char);
    if (explorer == nullptr) {
      return *this;
    }
  }

  if (!explorer->is_value_node_) {
    return *this;
  }

  std::shared_ptr<TrieNode> new_child(nullptr);
  if (!explorer->children_.empty()) {
    new_child = std::make_shared<TrieNode>(explorer->children_);
  }
  for (decltype(key.size()) i = key.size() - 1; !stk.empty(); i--) {
    auto pre_node = stk.top();
    stk.pop();

    char key_char = key[i];
    auto up_child = std::shared_ptr(pre_node->Clone());
    if (new_child == nullptr) {
      up_child->children_.erase(key_char);
    } else {
      up_child->children_[key_char] = new_child;
    }

    if (!up_child->is_value_node_ && up_child->children_.empty()) {
      up_child = nullptr;
    }
    new_child = up_child;
  }
  return Trie(new_child);

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
