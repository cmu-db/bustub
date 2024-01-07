#include "primer/orset.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "common/exception.h"

namespace bustub {

template <typename T>
auto ORSet<T>::Contains(const T &elem) const -> bool {
  throw NotImplementedException("OrSet<T>::Contains is not implemented.");
}

template <typename T>
auto ORSet<T>::Elements() const -> std::vector<T> {
  throw NotImplementedException("OrSet<T>::Elements is not implemented.");
}

template <typename T>
void ORSet<T>::Add(const T &elem) {
  throw NotImplementedException("OrSet<T>::Add is not implemented.");
}

template <typename T>
void ORSet<T>::Remove(const T &elem) {
  // 1. Prepare the observed elements.
  // 2. Remove the observed elements.

  throw NotImplementedException("OrSet<T>::Remove is not implemented.");
}

template <typename T>
void ORSet<T>::Merge(ORSet<T> &other) {
  throw NotImplementedException("OrSet<T>::Merge is not implemented.");
}

template class ORSet<int>;
template class ORSet<std::string>;

}  // namespace bustub
