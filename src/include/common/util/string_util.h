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
  static bool Contains(const std::string &haystack, const std::string &needle);

  /** @return true if target string starts with given prefix, false otherwise */
  static bool StartsWith(const std::string &str, const std::string &prefix);

  /** @return true if target string ends with the given suffix, false otherwise */
  static bool EndsWith(const std::string &str, const std::string &suffix);

  /** @return str repeated n times */
  static std::string Repeat(const std::string &str, std::size_t n);

  /** @return input string split based on the delimiter */
  static std::vector<std::string> Split(const std::string &str, char delimiter);

  /** @return concatenation of all input strings, separated by the separator */
  static std::string Join(const std::vector<std::string> &input, const std::string &separator);

  /** @return prefix prepended to the beginning of each line in str */
  static std::string Prefix(const std::string &str, const std::string &prefix);

  /** @return bytes formatted into the appropriate kilobyte, megabyte or gigabyte representation */
  static std::string FormatSize(uint64_t bytes);

  /** @return string wrapped with control characters to appear bold in the console */
  static std::string Bold(const std::string &str);

  /** @return uppercase version of the string */
  static std::string Upper(const std::string &str);

  /** @return lowercase version of the string */
  static std::string Lower(const std::string &str);

  /** @return string formatted with printf semantics */
  static std::string Format(std::string fmt_str, ...);

  /** @return input string split based on the split string */
  static std::vector<std::string> Split(const std::string &input, const std::string &split);

  /**
   * Removes the whitespace characters from the right side of the string.
   * @param[in,out] str string to be trimmed on the right
   */
  static void RTrim(std::string *str);

  /** @return indented string */
  static std::string Indent(int num_indent);

  /**
   * Return a new string that has stripped all occurrences of the provided character from the provided string.
   *
   * NOTE: WASTEFUL. Performs a copy. Do NOT use for performance-critical code!
   *
   * @param str input string
   * @param c character to be removed
   * @return a new string with no occurrences of the provided character
   */
  static std::string Strip(const std::string &str, char c);
};

}  // namespace bustub
