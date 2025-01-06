#include "primer/hyperloglog.h"

namespace bustub {

template <typename KeyType>
HyperLogLog<KeyType>::HyperLogLog(int16_t n_bits) :
  cardinality_(0), m_n_bits_(n_bits), m_buckets(1 << n_bits, 0), m_dividend(CONSTANT *  (1 << 2 * n_bits)), m_divisor(1 << m_n_bits_) {
}

template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {
  /** @TODO(student) Implement this function! */
  return {hash};
}

template <typename KeyType>
auto HyperLogLog<KeyType>::PositionOfLeftmostOne(const std::bitset<BITSET_CAPACITY> &bset) const -> uint64_t {
  /** @TODO(student) Implement this function! */
  for (auto i = BITSET_CAPACITY - 1 - m_n_bits_; i >= 0; i--) {
    if (bset[i]) {
      return BITSET_CAPACITY - m_n_bits_ - i;
    }
  }
  return 0;
}

template <typename KeyType>
auto HyperLogLog<KeyType>::AddElem(KeyType val) -> void {
  /** @TODO(student) Implement this function! */
  hash_t hash = CalculateHash(std::move(val));
  const auto binary_value = ComputeBinary(hash);
  size_t bucket_idx = CalcBucketIdx(binary_value);

  uint64_t original_value = m_buckets[bucket_idx];
  m_buckets[bucket_idx] = std::max(m_buckets[bucket_idx], PositionOfLeftmostOne(binary_value));
  if (UpdateDivisor(original_value, m_buckets[bucket_idx])) {
    ComputeCardinality();
  }
}

template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeCardinality() -> void {
  cardinality_ = m_dividend / m_divisor;
}

template <typename KeyType>
auto HyperLogLog<KeyType>::CalcBucketIdx(const std::bitset<BITSET_CAPACITY>& binary) const -> size_t {
  const std::bitset<BITSET_CAPACITY> mask = (~std::bitset<BITSET_CAPACITY>(0)) << (BITSET_CAPACITY - m_n_bits_);
  std::bitset<BITSET_CAPACITY> extracted = binary & mask;
  extracted >>= (BITSET_CAPACITY - m_n_bits_);
  return extracted.to_ulong();
}

template <typename KeyType>
bool HyperLogLog<KeyType>::UpdateDivisor(uint64_t original_value, uint64_t curr_value) {
  if (curr_value == original_value) return false;
  m_divisor += pow(2.0, -static_cast<double>(curr_value)) - pow(2.0, -static_cast<double>(original_value));
  return true;
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
