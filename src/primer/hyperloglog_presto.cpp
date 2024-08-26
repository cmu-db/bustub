#include "primer/hyperloglog_presto.h"

namespace bustub {

template <typename T>
auto HyperLogLogPresto<T>::AddElem(T val) -> void {
  /** @todo (student) has to fill the function */
}

template <typename T>
auto HyperLogLogPresto<T>::ComputeCardinality() -> void {
  // TODO(student) - implement the function
}

template class HyperLogLogPresto<int64_t>;
template class HyperLogLogPresto<std::string>;
}  // namespace bustub
