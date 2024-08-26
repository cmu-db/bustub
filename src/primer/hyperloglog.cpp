#include "primer/hyperloglog.h"

namespace bustub {

template <typename T>
auto inline HyperLogLog<T>::CalculateHash(T val) -> hash_t {
  /** @todo student -  implement the function */
  return 0;
}

template <typename T>
auto HyperLogLog<T>::ComputeBinary(const hash_t &hash) const -> std::bitset<MAX_BITS> {
  /** @todo student - implement the function */
  return {0};
}

template <typename T>
auto HyperLogLog<T>::PositionOfLeftmostOne(const std::bitset<MAX_BITS> &bset) const -> uint64_t {
  /** @todo student - implement the function*/
  return 0;
}

template <typename T>
auto HyperLogLog<T>::AddElem(T val) -> void {
  /** @todo implement the function */
}

template <typename T>
auto HyperLogLog<T>::ComputeCardinality() -> void {
  /** @todo - student implement the function */
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
