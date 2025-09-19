//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// count_min_sketch.cpp
//
// Identification: src/primer/count_min_sketch.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/count_min_sketch.h"

#include <atomic>
#include <memory>
#include <stdexcept>
#include <string>

namespace bustub {

/**
 * Constructor for the count-min sketch.
 *
 * @param width The width of the sketch matrix.
 * @param depth The depth of the sketch matrix.
 * @throws std::invalid_argument if width or depth are zero.
 */
template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(uint32_t width, uint32_t depth) : width_(width), depth_(depth) {
  if (width == 0 || depth == 0) {
    throw std::invalid_argument("Width and depth must be greater than zero.");
  }

  sketch_matrix_.resize(depth_);
  for (auto& row : sketch_matrix_) {
    row.reserve(width_);
    for (uint32_t j = 0; j < width_; j++) {
      row.emplace_back(std::make_shared<std::atomic_uint32_t>(0));
    }
  }


  /** @fall2025 PLEASE DO NOT MODIFY THE FOLLOWING */
  // Initialize seeded hash functions
  hash_functions_.reserve(depth_);
  for (size_t i = 0; i < depth_; i++) {
    hash_functions_.emplace_back(this->HashFunction(i));
  }
}

template <typename KeyType>
CountMinSketch<KeyType>::CountMinSketch(CountMinSketch &&other) noexcept
    : width_(other.width_),
      depth_(other.depth_),
      hash_functions_(std::move(other.hash_functions_)),
      sketch_matrix_(std::move(other.sketch_matrix_)) {
  other.width_ = 0;
  other.depth_ = 0;
}

template <typename KeyType>
auto CountMinSketch<KeyType>::operator=(CountMinSketch &&other) noexcept -> CountMinSketch & {
  if (this != &other) {
    width_ = other.width_;
    depth_ = other.depth_;
    hash_functions_ = std::move(other.hash_functions_);
    sketch_matrix_ = std::move(other.sketch_matrix_);

    other.width_ = 0;
    other.depth_ = 0;
  }
  return *this;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Insert(const KeyType &item) {
  for (auto &hash_fn : hash_functions_) {
    uint32_t idx = hash_fn(item) % width_;
    // Increment the count in the sketch matrix
    // Note: This is a simple implementation and does not handle overflow
    // In a production system, you might want to handle this case
    sketch_matrix_[&hash_fn - &hash_functions_[0]][idx]->fetch_add(1, std::memory_order_relaxed);
  }
}

template <typename KeyType>
void CountMinSketch<KeyType>::Merge(const CountMinSketch<KeyType> &other) {
  if (width_ != other.width_ || depth_ != other.depth_) {
    throw std::invalid_argument("Incompatible CountMinSketch dimensions for merge.");
  }
  for (uint32_t i = 0; i < depth_; i++) {
    for (uint32_t j = 0; j < width_; j++) {
      *sketch_matrix_[i][j] += *other.sketch_matrix_[i][j];
    }
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::Count(const KeyType &item) const -> uint32_t {
  uint32_t min_count = std::numeric_limits<uint32_t>::max();
  for (size_t i = 0; i < depth_; i++) {
    uint32_t idx = hash_functions_[i](item) % width_;
    auto cell = sketch_matrix_[i][idx];
    min_count = std::min(min_count, cell->load());
  }
  return min_count;
}

template <typename KeyType>
void CountMinSketch<KeyType>::Clear() {
  for (auto &row : sketch_matrix_) {
    for (auto &cell : row) {
      cell->store(0);
    }
  }
}

template <typename KeyType>
auto CountMinSketch<KeyType>::TopK(uint16_t k, const std::vector<KeyType> &candidates)
    -> std::vector<std::pair<KeyType, uint32_t>> {
  if (k > candidates.size()) {
    throw std::invalid_argument("k cannot be greater than the number of candidates.");
  }

  auto result = std::vector<std::pair<KeyType, uint32_t>>{};
  result.reserve(candidates.size());

  for (const auto &item : candidates) {
    result.emplace_back(item, Count(item));
  }

  std::partial_sort(result.begin(), result.begin() + std::min<size_t>(k, result.size()), result.end(),
                    [](const auto &a, const auto &b) { return a.second > b.second; });

  result.resize(k);
  return result;
}

// Explicit instantiations for all types used in tests
template class CountMinSketch<std::string>;
template class CountMinSketch<int64_t>;  // For int64_t tests
template class CountMinSketch<int>;      // This covers both int and int32_t
}  // namespace bustub
