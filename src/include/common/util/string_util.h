//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// string_util.h
//
// Identification: src/include/common/util/string_util.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <string>
#include <vector>

namespace bustub {

/**
 * StringUtil provides INEFFICIENT utility functions for working with strings. They should only be used for debugging.
 */
class StringUtil {
 public:
  /** @return true if haystack contains needle, false otherwise */
  static auto Contains(const std::string &haystack, const std::string &needle) -> bool;

  /** @return true if target string starts with given prefix, false otherwise */
  static auto StartsWith(const std::string &str, const std::string &prefix) -> bool;

  /** @return true if target string ends with the given suffix, false otherwise */
  static auto EndsWith(const std::string &str, const std::string &suffix) -> bool;

  /** @return str repeated n times */
  static auto Repeat(const std::string &str, std::size_t n) -> std::string;

  /** @return input string split based on the delimiter */
  static auto Split(const std::string &str, char delimiter) -> std::vector<std::string>;

  /** @return concatenation of all input strings, separated by the separator */
  static auto Join(const std::vector<std::string> &input, const std::string &separator) -> std::string;

  /** @return prefix prepended to the beginning of each line in str */
  static auto Prefix(const std::string &str, const std::string &prefix) -> std::string;

  /** @return bytes formatted into the appropriate kilobyte, megabyte or gigabyte representation */
  static auto FormatSize(uint64_t bytes) -> std::string;

  /** @return string wrapped with control characters to appear bold in the console */
  static auto Bold(const std::string &str) -> std::string;

  /** @return uppercase version of the string */
  static auto Upper(const std::string &str) -> std::string;

  /** @return lowercase version of the string */
  static auto Lower(const std::string &str) -> std::string;

  /** @return string formatted with printf semantics */
  static auto Format(std::string fmt_str, ...) -> std::string;

  /** @return input string split based on the split string */
  static auto Split(const std::string &input, const std::string &split) -> std::vector<std::string>;

  /**
   * Removes the whitespace characters from the right side of the string.
   * @param[in,out] str string to be trimmed on the right
   */
  static void RTrim(std::string *str);

  /** @return indented string */
  static auto Indent(int num_indent) -> std::string;

  /**
   * Return a new string that has stripped all occurrences of the provided character from the provided string.
   *
   * NOTE: WASTEFUL. Performs a copy. Do NOT use for performance-critical code!
   *
   * @param str input string
   * @param c character to be removed
   * @return a new string with no occurrences of the provided character
   */
  static auto Strip(const std::string &str, char c) -> std::string;
};

}  // namespace bustub
