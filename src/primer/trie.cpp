#include "primer/trie.h"
#include <string_view>
#include "common/exception.h"

namespace bustub {

template <class T>
auto Trie::Get(std::string_view key) const -> const T * {
  //throw NotImplementedException("Trie::Get is not implemented.");
  // You should walk through the trie to find the node corresponding to the key. If the node doesn't exist, return
  // nullptr. After you find the node, you should use `dynamic_cast` to cast it to `const TrieNodeWithValue<T> *`. If
  // dynamic_cast returns `nullptr`, it means the type of the value is mismatched, and you should return nullptr.
  // Otherwise, return the value.
  if(root_ == nullptr)
    return nullptr;
  std::shared_ptr<TrieNode> currentNode = root_->Clone();

  for(auto k :key){
    if(currentNode->children_.count(k) == 0){
        return nullptr;
    }
    currentNode = currentNode->children_[k]->Clone();
  }


  if(!currentNode->is_value_node_)
    return nullptr;

  auto valueNode = std::dynamic_pointer_cast<TrieNodeWithValue<T>>(currentNode);

  //auto flag = std::dynamic_pointer_cast<TrieNodeWithValue<T>>()



  if (valueNode ) {
    if(valueNode->is_value_node_)
    return valueNode->value_.get();
  }

  return nullptr;
}

template <class T>
auto Trie::Put(std::string_view key, T value) const -> Trie {

  std::shared_ptr<const TrieNode> newRoot = root_;
  if(newRoot == nullptr){
    newRoot = std::make_shared<TrieNode>();
  }

  std::shared_ptr<TrieNode> currentNode = newRoot->Clone();
  std::shared_ptr<TrieNode> saveNode = currentNode;
  std::shared_ptr<TrieNode> pre_ptr = saveNode;

  if(key.size() == 0){
    return Trie(std::make_shared<TrieNodeWithValue<T>>(currentNode->children_,std::move(std::make_shared<T>(std::move(value)))));
  }
  for (auto k : key) {
    if (currentNode->children_.count(k) == 0) {
      auto newNode = std::make_shared<TrieNode>();
      currentNode->children_[k] = std::move(newNode);
    }
    pre_ptr = currentNode;
    currentNode = currentNode->children_[k]->Clone();
    pre_ptr->children_[k] = currentNode;
  }
  //auto oldtree = std::dynamic_pointer_cast<TrieNodeWithValue<T>>(root_.);
    auto newValueNode = std::make_shared<TrieNodeWithValue<T>>(currentNode->children_,std::move(std::make_shared<T>(std::move(value))));
    currentNode = newValueNode;
    pre_ptr->children_[key[key.size() - 1]] = currentNode;

  return Trie(saveNode);

}

auto Trie::Remove(std::string_view key) const -> Trie {
  //throw NotImplementedException("Trie::Remove is not implemented.");

  // You should walk through the trie and remove nodes if necessary. If the node doesn't contain a value any more,
  // you should convert it to `TrieNode`. If a node doesn't have children any more, you should remove it.
  if(root_ == nullptr) return Trie();

  std::shared_ptr<TrieNode> cur = root_->Clone();
  std::shared_ptr<TrieNode> save = cur;
  std::shared_ptr<TrieNode> pre =  cur;
  for(auto k : key)
  {
    if(cur->children_.count(k) == 0){
      return Trie(save);
    }
    pre = cur;
    cur = cur->children_[k]->Clone();
    pre->children_[k] = cur;
  }
  auto newValue = std::make_shared<TrieNode>(cur->children_);
  pre->children_[key[key.size()-1]] = newValue;
  return Trie(save);
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
