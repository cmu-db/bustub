//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// orset_driver.h
//
// Identification: src/include/primer/orset_driver.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <vector>
#include "primer/orset.h"

namespace bustub {

/** @brief Unique ID type. */
using uid_t = int64_t;

template <typename T>
class ORSetDriver;

template <typename T>
class ORSetNode {
 public:
  ORSetNode() = delete;

  explicit ORSetNode(ORSetDriver<T> *driver, size_t node_id, size_t n)
      : driver_(driver), node_id_(node_id), peer_size_(n), last_read_version_(n, 0) {}

  /**
   * @brief Adds an element to the local ORSet.
   *
   * @param elem the element to add
   */
  inline void Add(const T &elem) { orset_.Add(elem, driver_->GenerateUid()); }

  /**
   * @brief Removes an element from the local ORSet.
   *
   * @param elem the element to remove.
   */
  inline void Remove(const T &elem) { orset_.Remove(elem); }

  /**
   * @brief Checks if an element is in the local ORSet.
   *
   * @param elem the element to check
   * @return true if the element is in the set, and false otherwise.
   */
  inline auto Contains(const T &elem) -> bool { return orset_.Contains(elem); }

  /**
   * @brief Merges another ORSet to the local ORSet.
   *
   * @param to_be_merged the ORSet to be merged.
   */
  inline void Merge(const ORSet<T> to_be_merged) { orset_.Merge(to_be_merged); }

  void Save();

  void Load();

  /**
   * @brief Gets a copy of the local ORSet.
   *
   * @return the local ORSet.
   */
  inline auto GetORSet() -> ORSet<T> { return orset_; }

 private:
  /** @brief The local ORSet. */
  ORSet<T> orset_;

  /** @brief ORSet Driver. */
  ORSetDriver<T> *driver_;

  /** @brief node id */
  size_t node_id_;

  /** @brief total number of nodes in the same network */
  size_t peer_size_;

  /** @brief last read version number of each peer's copy */
  std::vector<uint32_t> last_read_version_;
};

/** @brief A driver class for managing ORSets. */
template <typename T>
class ORSetDriver {
  friend class ORSetNode<T>;

 public:
  explicit ORSetDriver(size_t num_orset_node);

  /**
   * @brief Gets the ORSetNode at index.
   */
  inline auto operator[](size_t index) -> std::unique_ptr<ORSetNode<T>> & { return orset_nodes_[index]; }
  auto operator[](size_t index) const -> const std::unique_ptr<ORSetNode<T>> & { return orset_nodes_[index]; }

  /**
   * @brief Gets the ORSet node at index.
   *
   * @param index index of the ORSet node.
   * @return the ORSet node associated with the index.
   */
  inline auto At(size_t index) -> std::unique_ptr<ORSetNode<T>> & { return orset_nodes_[index]; }

  void Sync();

 private:
  /**
   * @brief Generates a unique id.
   *
   * @return a unique id.
   */
  inline auto GenerateUid() -> uid_t { return next_uid_++; }

  /** @brief A list of ORSet nodes. */
  std::vector<std::unique_ptr<ORSetNode<T>>> orset_nodes_;

  /** @brief  List of saved copies of ORSet. */
  std::vector<ORSet<T>> saved_copies_;

  /** @brief latest version number of each node */
  std::vector<uint32_t> version_counter_;

  /** @brief Monotonically increasing unique id for the elements. */
  uid_t next_uid_ = 0;
};

}  // namespace bustub
