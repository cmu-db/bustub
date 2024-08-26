#pragma once

#include <bitset>
#include <functional>
#include <memory>
#include <mutex>  // NOLINT
#include <string>
#include <unordered_map>
#include <vector>

#define BUCKET_SIZE 4UL
#define CONSTANT 0.79402
#define MAX_SIZE 7UL

namespace bustub {

template <typename T>
class HyperLogLogPresto {
  /**
   * INSTRUCTIONS: Testing framework will use the GetDenseBucket and GetOverflow function,
   * hence SHOULD NOT be deleted. It's essential to use the dense_bucket_
   * data structure.
   */

  /** @brief Hash type. */
  using hash_t = uint64_t;

 public:
  /** @brief Disabling default constructor. */
  HyperLogLogPresto() = delete;

  /** @brief Parameterized constructor. */
  explicit HyperLogLogPresto(int16_t n_leading_bits) : cardinality_(0) {}

  /** @brief Returns the dense_bucket_ data structure. */
  auto GetDenseBucket() const -> std::vector<std::bitset<BUCKET_SIZE>> { return dense_bucket_; }

  /** @brief Returns overflow bucket of a specific given index. */
  auto GetOverflowBucketofIndex(uint16_t idx) { return overflow_bucket_[idx]; }

  /** @brief Retusn the cardinality of the set. */
  auto GetCardinality() const -> uint64_t { return cardinality_; }

  /** @brief Element is added for HLL calculation. */
  auto AddElem(T val) -> void;

  /** @brief Function to compute cardinality. */
  auto ComputeCardinality() -> void;

 private:
  /** @brief Calculate Hash.
   *
   * @param[in] val
   *
   * @returns hash value
   */
  inline auto CalculateHash(T val) -> hash_t { return std::hash<T>{}(val); }

  /** @brief Structure holding dense buckets (or also known as registers). */
  std::vector<std::bitset<BUCKET_SIZE>> dense_bucket_;

  /** @brief Structure holding overflow buckets. */
  std::unordered_map<uint16_t, std::bitset<MAX_SIZE - BUCKET_SIZE>> overflow_bucket_;

  /** @brief Storing cardinality value */
  uint64_t cardinality_;

  // TODO(student) - can add more data structures as required
};

}  // namespace bustub
