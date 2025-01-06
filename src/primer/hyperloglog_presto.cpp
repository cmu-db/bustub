#include "primer/hyperloglog_presto.h"

#include <utility>

namespace bustub {

template <typename KeyType>
HyperLogLogPresto<KeyType>::HyperLogLogPresto(int16_t n_leading_bits) : cardinality_(0),m_n_leading_bits(n_leading_bits),
m_dividend(CONSTANT *  (1 << 2 * n_leading_bits)), m_divisor(1 << n_leading_bits) {
  size_t bucket_size = 1 << n_leading_bits;
  this.dense_bucket_(bucket_size, std::bitset<DENSE_BUCKET_SIZE>(0));

}

template <typename KeyType>
auto HyperLogLogPresto<KeyType>::AddElem(KeyType val) -> void {
  /** @TODO(student) Implement this function! */

  // 1. Compute the binary representation of the hash value
  const auto binary = ComputeBinary(CalculateHash(std::move(val)));

  // 2. Determine the bucket index based on the binary value
  const auto bucket_id = CalculateBucketId(binary);

  // 3. Get the least significant bit (LSB) from the binary value
  const auto lsb = ComputeLSB(binary);

  // Create a bitset for the dense and overflowing values
  std::bitset<DENSE_BUCKET_SIZE> dense_binary = std::bitset<DENSE_BUCKET_SIZE>(lsb);
  std::bitset<OVERFLOW_BUCKET_SIZE> overflow_binary = std::bitset<OVERFLOW_BUCKET_SIZE>(lsb >> dense_binary);

  // 4. Use lazy updates to maintain the current baseline
  // If the baseline is updated, also update the next
  const auto prev_dense_binary = dense_bucket_[bucket_id];
  const auto prev_baseline = *std::min_element(dense_bucket_.begin(), dense_bucket_.end());

  bool should_update_single_bucket = false;
  if (dense_binary > prev_dense_binary) {
    dense_bucket_[bucket_id] = dense_binary;
    should_update_single_bucket = true;
  }

  // If the dense binary value exceeds the previous baseline, we update all buckets
  const auto new_baseline = *std::min_element(dense_bucket_.begin(), dense_bucket_.end());

  // TODO: implement R(j) update
  bool updated_all_buckets = false;

  // TODO: consider lazy updating
  if (new_baseline.to_ulong() > prev_baseline.to_ulong()) {
    updated_all_buckets = true;
    // Update all buckets with the new baseline
    UpdateDenseBucketsWithNewBaseLine(new_baseline.to_ulong() - prev_baseline.to_ulong(), dense_bucket_);
  }

  // If a single bucket was updated but the baseline didn't change, perform additional handling
  if (!updated_all_buckets && should_update_single_bucket) {
    // Additional logic for handling the case where only one bucket was updated
  }
}



template <typename T>
auto HyperLogLogPresto<T>::ComputeCardinality() -> void {
  cardinality_ = m_dividend / m_divisor;
}

template <typename KeyType>
auto HyperLogLogPresto<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {
  return {hash};
}

template <typename KeyType>
auto HyperLogLogPresto<KeyType>::CompuateLSB(std::bitset<BITSET_CAPACITY> binary) const {
  for (size_t i = 0; i < BITSET_CAPACITY; i++) {
    if (binary[i] == 1) {
      return i;
    }
  }
  return 0;
}

template <typename KeyType>
auto HyperLogLogPresto<KeyType>::CalcBucketIdx(const std::bitset<BITSET_CAPACITY>& binary) const -> size_t {
  const std::bitset<BITSET_CAPACITY> mask = (~std::bitset<BITSET_CAPACITY>(0)) << (BITSET_CAPACITY - m_n_leading_bits);
  std::bitset<BITSET_CAPACITY> extracted = binary & mask;
  extracted >>= (BITSET_CAPACITY - m_n_leading_bits);
  return extracted.to_ulong();
}


template <typename KeyType>
// TODO: lazy updating
void HyperLogLogPresto<KeyType>::UpdateDenseBucketsWithNewBaseLine(long long delta,
  std::vector<std::bitset<DENSE_BUCKET_SIZE>> dense_bucket) {
  for (auto& reg : dense_bucket) {
    auto current_value = reg.to_ulong();
    current_value += delta;
    reg = std::bitset<DENSE_BUCKET_SIZE>(current_value);
  }
}

template class HyperLogLogPresto<int64_t>;
template class HyperLogLogPresto<std::string>;
}  // namespace bustub

