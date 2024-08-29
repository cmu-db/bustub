#pragma once

#include <bitset>
#include <memory>
#include <mutex>  // NOLINT
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "primer/primer_hash.h"

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
  explicit HyperLogLogPresto(int16_t n_leading_bits, const PrimerHashFunction<T> &hash_fn)
      : cardinality_(0), hash_fn_(std::move(hash_fn)) {}

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
  inline auto CalculateHash(T val) -> hash_t {
    if (std::is_same<T, std::string>::value) {
      return hash_fn_.GetHash(val);
    }
    /** @brief Convert into output stream */
    std::ostringstream o_stream;

    o_stream << val;  // output stream

    /** @brief Output stream result. */
    std::string res = o_stream.str();

    return static_cast<hash_t>(std::stoul(res));
  }

  /** @brief Structure holding dense buckets (or also known as registers). */
  std::vector<std::bitset<BUCKET_SIZE>> dense_bucket_;

  /** @brief Structure holding overflow buckets. */
  std::unordered_map<uint16_t, std::bitset<MAX_SIZE - BUCKET_SIZE>> overflow_bucket_;

  /** @brief Storing cardinality value */
  uint64_t cardinality_;

  /** @brief Hash Function. */
  PrimerHashFunction<T> hash_fn_;

  // TODO(student) - can add more data structures as required
};

}  // namespace bustub
