//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// orset.h
//
// Identification: src/include/primer/orset.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

namespace bustub {

/** @brief Unique ID type. */
using uid_t = int64_t;

/** @brief The observed remove set datatype. */
template <typename T>
class ORSet {
 public:
  ORSet() = default;

  auto Contains(const T &elem) const -> bool;

  void Add(const T &elem, uid_t uid);

  void Remove(const T &elem);

  void Merge(const ORSet<T> &other);

  auto Elements() const -> std::vector<T>;

  auto ToString() const -> std::string;

 private:
  // TODO(student): Add your private memeber variables to represent ORSet.
};

}  // namespace bustub
