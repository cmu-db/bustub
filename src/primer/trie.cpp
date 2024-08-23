#include "primer/trie.h"
#include <stack>
#include <string_view>
#include "common/exception.h"
namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  // throw NotImplementedException("Trie::Get is not implemented.");
  std::shared_ptr<const TrieNode> curr = root_;
  if (!curr) {
    return nullptr;
  }
  for (char c : key) {
    auto it = curr->children_.find(c);
    if (it == curr->children_.end()) {
      return nullptr;
    }
    curr = it->second;
  }

  if (curr && curr->is_value_node_) {
    auto value_node = dynamic_cast<const TrieNodeWithValue<T> *>(curr.get());
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

  // You should walk through the trie and create new nodes if necessary. If the node corresponding to the key already
  // exists, you should create a new `TrieNodeWithValue`.

  if (!root_) {
    // Handle the empty trie case
    std::shared_ptr<TrieNode> current_node =
        std::make_shared<TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
    for (auto it = key.rbegin(); it != key.rend(); ++it) {
      auto new_node =
          std::make_shared<TrieNode>(std::map<char, std::shared_ptr<const TrieNode>>({{*it, current_node}}));
      current_node = std::move(new_node);
    }
    return Trie{current_node};
  }

  std::stack<std::shared_ptr<const TrieNode>> node_stack;

  auto node = root_;
  auto it = key.begin();
  while (it != key.end()) {
    node_stack.push(node);
    auto child = node->children_.find(*it);
    if (child == node->children_.end()) {
      break;
    }
    node = child->second;
    ++it;
  }

  if (it == key.end()) {
    // Handle the case when key is a substring of the prefix
    std::shared_ptr<const TrieNode> current_node =
        std::make_shared<const TrieNodeWithValue<T>>(node->children_, std::make_shared<T>(std::move(value)));

    for (auto rit = key.rbegin(); rit != key.rend(); ++rit) {
      auto new_parent = node_stack.top()->Clone();
      new_parent->children_[*rit] = current_node;
      node_stack.pop();
      current_node = std::move(new_parent);
    }

    return Trie{current_node};
  }

  // Handle the case when prefix is a substring of the key
  auto rit = key.rbegin();

  std::shared_ptr<const TrieNode> current_node =
      std::make_shared<const TrieNodeWithValue<T>>(std::make_shared<T>(std::move(value)));
  while (std::distance(it, rit.base()) > 1) {
    auto new_node = std::make_shared<TrieNode>(std::map<char, std::shared_ptr<const TrieNode>>({{*rit, current_node}}));
    current_node = new_node;
    rit++;
  }

  while (!node_stack.empty() && rit != key.rend()) {
    auto old_parent = node_stack.top();
    auto new_node = old_parent->Clone();
    new_node->children_[*rit] = current_node;
    rit++;
    node_stack.pop();
    current_node = std::move(new_node);
  }

  return Trie{current_node};
}

auto Trie::Remove(std::string_view key) const -> Trie {
  // throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  if (!root_) {
    return Trie{};
  }

  std::stack<std::shared_ptr<const TrieNode>> node_stack;

  auto node = root_;
  for (char it : key) {
    node_stack.push(node);
    auto child = node->children_.find(it);
    if (child == node->children_.end()) {
      return Trie{root_};
    }
    node = child->second;
  }

  if (!node->is_value_node_) {
    return Trie{root_};
  }

  node = std::make_shared<const TrieNode>(node->children_);

  for (auto rit = key.rbegin(); rit != key.rend(); ++rit) {
    auto new_parent = node_stack.top()->Clone();
    if (node->children_.empty() && !node->is_value_node_) {
      new_parent->children_.erase(*rit);
    } else {
      new_parent->children_[*rit] = node;
    }
    node = std::move(new_parent);
    node_stack.pop();
  }

  if (node->children_.empty()) {
    return Trie{};
  }
  return Trie{node};
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
