//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hyperloglog.cpp
//
// Identification: src/primer/hyperloglog.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/hyperloglog.h"

namespace bustub {

/** @brief Parameterized constructor. */
template <typename KeyType>
HyperLogLog<KeyType>::HyperLogLog(int16_t n_bits) : cardinality_(0) {
  if (n_bits >= 0) {
    n_bits_ = n_bits;
    registers_.resize(1<<n_bits_);
  }
}

/**
 * @brief Function that computes binary.
 *
 * @param[in] hash
 * @returns binary of a given hash
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {
  /** @TODO(student) Implement this function! */
  std::bitset<BITSET_CAPACITY> bset;
  for (int i = 0; i < BITSET_CAPACITY; ++i) {
    bset[i] = (hash >> i) & 1;
  }
  return bset;
}

/**
 * @brief Function that computes leading zeros.
 *
 * @param[in] bset - binary values of a given bitset
 * @returns leading zeros of given binary set
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::PositionOfLeftmostOne(const std::bitset<BITSET_CAPACITY> &bset) const -> uint64_t {
  /** @TODO(student) Implement this function! */
  for (uint64_t i = bset.size() - 1; i >=0; --i) {
    if (bset[i]) {
      return i;
    }
  }
  return 0;
}

/**
 * @brief Adds a value into the HyperLogLog.
 *
 * @param[in] val - value that's added into hyperloglog
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::AddElem(KeyType val) -> void {
  /** @TODO(student) Implement this function! */
  auto hash = CalculateHash(val);
  auto bset = ComputerBinary(hash);

  auto register_idx = (bset >> BITSET_CAPACITY - n_bits_).to_ullong();

  for (size_t i = 0; i < register_idx; i++) {
    bset[BITSET_CAPACITY - 1 - i] = false;
  }

  uint64_t p_val = BITSET_CAPACITY - PositionOfLeftmostOne(bset) - n_bits_;

  register_[register_idx] = std::max(registers_[register_idx], p_val);
}

/**
 * @brief Function that computes cardinality.
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeCardinality() -> void {
  /** @TODO(student) Implement this function! */
  if (register_.empty()) {
    cardinality_ = 0;
    return;
  }

  double sum = 0.0;
  for (const auto &reg: register_) {
    sum += 1.0 / (1 << reg);
  }

  double m = 1ULL << n_bits_;
  double estimate = CONSTANT * m * m * (1.0/sum);
  cardinality_ = static_cast<uint64_t>(std::floor(estimate));
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
