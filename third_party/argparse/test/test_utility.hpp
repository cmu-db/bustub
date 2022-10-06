#ifndef ARGPARSE_TEST_UTILITY_HPP
#define ARGPARSE_TEST_UTILITY_HPP

namespace testutility {
// Get value at index from std::list
template <typename T>
T get_from_list(const std::list<T>& aList, size_t aIndex) {
  if (aList.size() > aIndex) {
    auto tIterator = aList.begin();
    std::advance(tIterator, aIndex);
    return *tIterator;
  }
  return T();
}
}

#endif //ARGPARSE_TEST_UTILITY_HPP
