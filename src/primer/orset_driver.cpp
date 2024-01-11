#include "primer/orset_driver.h"
#include <string>

namespace bustub {

template <typename T>
ORSetDriver<T>::ORSetDriver(size_t num_orset_node) {
  orset_nodes_.reserve(num_orset_node);
  for (size_t i = 0; i < num_orset_node; ++i) {
    orset_nodes_.emplace_back(std::make_unique<ORSetNode<T>>(this, i));
  }
  saved_copies_.resize(num_orset_node);
}

template <typename T>
void ORSetNode<T>::Load() {
  for (size_t i = 0; i < driver_->saved_copies_.size(); ++i) {
    if (i == node_id_) {
      continue;
    }
    Merge(driver_->saved_copies_[i]);
  }
}

template <typename T>
void ORSetNode<T>::Save() {
  driver_->saved_copies_[node_id_] = orset_;
}

template <typename T>
void ORSetDriver<T>::Sync() {
  for (const auto &node : orset_nodes_) {
    node->Save();
  }
  for (const auto &node : orset_nodes_) {
    node->Load();
  }
}

template class ORSetDriver<int>;
template class ORSetDriver<std::string>;

}  // namespace bustub
