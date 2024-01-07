#pragma once

#include <atomic>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "common/util/hash_util.h"
#include "fmt/format.h"

namespace bustub {

/** @brief Unique ID type. */
using uid_t = int64_t;

/** @brief Monotonically increasing unique id. */
static std::atomic<uid_t> next_uid = 0;

/**
 * @brief Generates a unique id.
 *
 * @return a unique id.
 */
inline auto ORSetGenerateUid() -> uid_t { return next_uid++; }

/** @brief The observed remove set datatype. */
template <typename T>
class ORSet {
 public:
  ORSet() = default;

  /**
   * @brief Checks if an element is in the set.
   *
   * @param elem the element to check
   * @return true if the element is in the set, and false otherwise.
   */
  auto Contains(const T &elem) const -> bool;

  /**
   * @brief Gets all the elements in the set.
   *
   * @return all the elements in the set.
   */
  auto Elements() const -> std::vector<T>;

  /**
   * @brief Adds an element to the set.
   *
   * @param elem the element to add
   */
  void Add(const T &elem);

  /**
   * @brief Removes an element from the set if it exists.
   *
   * @param elem the element to remove.
   */
  void Remove(const T &elem);

  /**
   * @brief Merge changes from another ORSet.
   *
   * @param other another ORSet
   */
  void Merge(ORSet<T> &other);

 private:
  /** @brief Set of elements (e, n). */
  std::unordered_map<T, std::unordered_set<uid_t>> elements_;
  /** @brief Set of tombstones (e, n). */
  std::unordered_map<T, std::unordered_set<uid_t>> tombstones_;
};

}  // namespace bustub
