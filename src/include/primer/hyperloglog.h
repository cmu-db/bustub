#pragma once

#include <bitset>
#include <functional>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <vector>

#define MAX_BITS 64
#define DEFAULT_CARDINALITY 0

namespace bustub {

/** @brief hash value type */
using hash_t = uint64_t;

/** @brief constant */
static const double CONSTANT = 0.79402;

template <typename T>
class HyperLogLog {
 public:
  HyperLogLog() = delete;

  explicit HyperLogLog(int16_t n_bits) {}

  /**
   * @brief getter value for cardinality
   *
   * @returns cardinality value
   */
  auto GetCardinality() { return cardinality_; }

  /**
   * @brief adds a value into the HyperLogLog
   *
   * @param[in] val - value that's added into hyperloglog
   */
  auto AddElem(T val) -> void;

  /**
   * @brief function that computes cardinality
   */
  auto ComputeCardinality() -> void;

 private:
  /**
   * @brief Calculates Hash of a given value
   *
   * @param[in] val - value
   * @returns hash integer of given input value
   */
  inline auto CalculateHash(T val) -> hash_t;

  /**
   * @brief function that computes binary
   *
   *
   * @param[in] hash
   * @returns binary of a given hash
   */
  auto ComputeBinary(const hash_t &hash) const -> std::bitset<MAX_BITS>;

  /**
   * @brief function that computes leading zeros
   *
   * @param[in] bset - binary values of a given bitset
   * @returns leading zeros of given binary set
   */
  auto PositionOfLeftmostOne(const std::bitset<MAX_BITS> &bset) const -> uint64_t;

  /** @brief cardinality value*/
  size_t cardinality_;

  /** @brief number of counters */
  uint64_t counters_;

  /** @todo Students must add their data structures that support HyperLogLog */
};

}  // namespace bustub
