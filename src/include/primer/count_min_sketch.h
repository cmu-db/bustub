//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// count_min_sketch.h
//
// Identification: src/include/primer/count_min_sketch.h
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include "common/util/hash_util.h"

namespace bustub {

template <typename KeyType>
class CountMinSketch {
 public:
  /** @brief Constructs a count-min sketch with specified dimensions
   * @param width Number of buckets
   * @param depth Number of hash functions
   */
  explicit CountMinSketch(uint32_t width, uint32_t depth);

  CountMinSketch() = delete;                                            // Default constructor deleted
  CountMinSketch(const CountMinSketch &) = delete;                      // Copy constructor deleted
  auto operator=(const CountMinSketch &) -> CountMinSketch & = delete;  // Copy assignment deleted

  CountMinSketch(CountMinSketch &&other) noexcept;                      // Move constructor
  auto operator=(CountMinSketch &&other) noexcept -> CountMinSketch &;  // Move assignment

  /**
   * @brief Inserts an item into the count-min sketch
   *
   * @param item The item to increment the count for
   * @note Updates the min-heap at the same time
   */
  void Insert(const KeyType &item);

  /**
   * @brief Gets the estimated count of an item
   *
   * @param item The item to look up
   * @return The estimated count
   */
  auto Count(const KeyType &item) const -> uint32_t;

  /**
   * @brief Resets the sketch to initial empty state
   *
   * @note Clears the sketch matrix, item set, and top-k min-heap
   */
  void Clear();

  /**
   * @brief Merges the current CountMinSketch with another, updating the current sketch
   * with combined data from both sketches.
   *
   * @param other The other CountMinSketch to merge with.
   * @throws std::invalid_argument if the sketches' dimensions are incompatible.
   */
  void Merge(const CountMinSketch<KeyType> &other);

  /**
   * @brief Gets the top k items based on estimated counts from a list of candidates.
   *
   * @param k Number of top items to return (will be capped at initial k)
   * @param candidates List of candidate items to consider for top k
   * @return Vector of (item, count) pairs in descending count order
   */
  auto TopK(uint16_t k, const std::vector<KeyType> &candidates) -> std::vector<std::pair<KeyType, uint32_t>>;

 private:
  /** Dimensions of the count-min sketch matrix */
  uint32_t width_;  // Number of buckets for each hash function
  uint32_t depth_;  // Number of independent hash functions
  /** Pre-computed hash functions for each row */
  std::vector<std::function<size_t(const KeyType &)>> hash_functions_;

  /** @spring2026 PLEASE DO NOT MODIFY THE FOLLOWING */
  constexpr static size_t SEED_BASE = 15445;

  /**
   * @brief Seeded hash function generator
   *
   * @param seed Used for creating independent hash functions
   * @return A function that maps items to column indices
   */
  inline auto HashFunction(size_t seed) -> std::function<size_t(const KeyType &)> {
    return [seed, this](const KeyType &item) -> size_t {
      auto h1 = std::hash<KeyType>{}(item);
      auto h2 = bustub::HashUtil::CombineHashes(seed, SEED_BASE);
      return bustub::HashUtil::CombineHashes(h1, h2) % width_;
    };
  }

  /** @todo (student) can add their data structures that support count-min sketch operations */
};

}  // namespace bustub
