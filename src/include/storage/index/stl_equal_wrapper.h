#pragma once

namespace bustub {

template <typename K, typename KC>
class StlEqualWrapper {
 public:
  explicit StlEqualWrapper(const KC &cmp) : cmp_{cmp} {}

  inline auto operator()(const K &lhs, const K &rhs) const -> bool { return cmp_(lhs, rhs) == 0; }

  KC cmp_;
};

}  // namespace bustub
