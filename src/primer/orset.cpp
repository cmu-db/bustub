//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// orset.cpp
//
// Identification: src/primer/orset.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/orset.h"
#include <algorithm>
#include <string>
#include <vector>
#include "common/exception.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace bustub {

/**
 * @brief Checks if an element is in the set.
 *
 * @param elem the element to check
 * @return true if the element is in the set, and false otherwise.
 */
template <typename T>
auto ORSet<T>::Contains(const T &elem) const -> bool {
  // TODO(student): Implement this
  throw NotImplementedException("ORSet<T>::Contains is not implemented");
}

/**
 * @brief Adds an element to the set.
 *
 * @param elem the element to add
 * @param uid unique token associated with the add operation.
 */
template <typename T>
void ORSet<T>::Add(const T &elem, uid_t uid) {
  // TODO(student): Implement this
  throw NotImplementedException("ORSet<T>::Add is not implemented");
}

/**
 * @brief Removes an element from the set if it exists.
 *
 * @param elem the element to remove.
 */
template <typename T>
void ORSet<T>::Remove(const T &elem) {
  // TODO(student): Implement this
  throw NotImplementedException("ORSet<T>::Remove is not implemented");
}

/**
 * @brief Merge changes from another ORSet.
 *
 * @param other another ORSet
 */
template <typename T>
void ORSet<T>::Merge(const ORSet<T> &other) {
  // TODO(student): Implement this
  throw NotImplementedException("ORSet<T>::Merge is not implemented");
}

/**
 * @brief Gets all the elements in the set.
 *
 * @return all the elements in the set.
 */
template <typename T>
auto ORSet<T>::Elements() const -> std::vector<T> {
  // TODO(student): Implement this
  throw NotImplementedException("ORSet<T>::Elements is not implemented");
}

/**
 * @brief Gets a string representation of the set.
 *
 * @return a string representation of the set.
 */
template <typename T>
auto ORSet<T>::ToString() const -> std::string {
  auto elements = Elements();
  std::sort(elements.begin(), elements.end());
  return fmt::format("{{{}}}", fmt::join(elements, ", "));
}

template class ORSet<int>;
template class ORSet<std::string>;

}  // namespace bustub
