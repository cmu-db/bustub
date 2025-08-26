//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// orset_driver.cpp
//
// Identification: src/primer/orset_driver.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/orset_driver.h"
#include <string>

namespace bustub {

/**
 * @brief Loads all the remote changes to the local ORSet.
 */
template <typename T>
void ORSetNode<T>::Load() {
  for (size_t i = 0; i < peer_size_; ++i) {
    if (i == node_id_) {
      continue;
    }
    uint32_t curr_version = driver_->version_counter_[i];
    if (last_read_version_[i] < curr_version) {
      Merge(driver_->saved_copies_[i]);
      last_read_version_[i] = curr_version;
    }
  }
}

/**
 * @brief Saves all local changes to the driver.
 */
template <typename T>
void ORSetNode<T>::Save() {
  driver_->saved_copies_[node_id_] = orset_;
  driver_->version_counter_[node_id_]++;
}

template <typename T>
ORSetDriver<T>::ORSetDriver(size_t num_orset_node) : version_counter_(num_orset_node) {
  orset_nodes_.reserve(num_orset_node);
  for (size_t i = 0; i < num_orset_node; ++i) {
    orset_nodes_.emplace_back(std::make_unique<ORSetNode<T>>(this, i, num_orset_node));
    version_counter_[i] = 0;
  }
  saved_copies_.resize(num_orset_node);
}

/**
 * @brief Saves changes in all nodes and then load all the changes.
 */
template <typename T>
void ORSetDriver<T>::Sync() {
  for (const auto &node : orset_nodes_) {
    node->Save();
  }
  for (const auto &node : orset_nodes_) {
    node->Load();
  }
}

template class ORSetNode<int>;
template class ORSetNode<std::string>;
template class ORSetDriver<int>;
template class ORSetDriver<std::string>;

}  // namespace bustub
