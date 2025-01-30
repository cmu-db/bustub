//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// string_util.cpp
//
// Identification: src/common/util/string_util.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "common/util/string_util.h"
#include "fmt/format.h"
#include "fmt/ranges.h"

namespace bustub {

/** @return true if haystack contains needle, false otherwise */
auto StringUtil::Contains(const std::string &haystack, const std::string &needle) -> bool {
  return (haystack.find(needle) != std::string::npos);
}

/** @return true if haystack contains needle after keyword, false otherwise */
auto StringUtil::ContainsAfter(const std::string &keyword, const std::string &haystack, const std::string &needle)
    -> bool {
  auto pos = haystack.find(keyword);
  if (pos == std::string::npos) {
    return false;
  }
  return (haystack.find(needle, pos) != std::string::npos);
}

/**
 * Removes the whitespace characters from the right side of the string.
 * @param[in,out] str string to be trimmed on the right
 */
void StringUtil::RTrim(std::string *str) {
  // remove trailing ' ', \f, \n, \r, \t, \v
  str->erase(std::find_if(str->rbegin(), str->rend(), [](int ch) { return std::isspace(ch) == 0; }).base(), str->end());
}

/**
 * Removes the whitespace characters from the left side of the string.
 * @param[in,out] str string to be trimmed on the left
 */
void StringUtil::LTrim(std::string *str) {
  // remove leading ' ', \f, \n, \r, \t, \v
  str->erase(str->begin(), std::find_if(str->begin(), str->end(), [](int ch) { return std::isspace(ch) == 0; }));
}

/** @return indented string */
auto StringUtil::Indent(int num_indent) -> std::string { return std::string(num_indent, ' '); }  // NOLINT

/** @return true if target string starts with given prefix, false otherwise */
auto StringUtil::StartsWith(const std::string &str, const std::string &prefix) -> bool {
  return std::equal(prefix.begin(), prefix.end(), str.begin());
}

/** @return true if target string ends with the given suffix, false otherwise */
auto StringUtil::EndsWith(const std::string &str, const std::string &suffix) -> bool {
  // http://stackoverflow.com/a/2072890
  if (suffix.size() > str.size()) {
    return false;
  }
  return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

/** @return str repeated n times */
auto StringUtil::Repeat(const std::string &str, const std::size_t n) -> std::string {
  std::ostringstream os;
  if (n == 0 || str.empty()) {
    return (os.str());
  }
  for (int i = 0; i < static_cast<int>(n); i++) {
    os << str;
  }
  return (os.str());
}

/** @return input string split based on the delimiter */
auto StringUtil::Split(const std::string &str, char delimiter) -> std::vector<std::string> {
  std::stringstream ss(str);
  std::vector<std::string> lines;
  std::string temp;
  while (std::getline(ss, temp, delimiter)) {
    lines.push_back(temp);
  }
  return (lines);
}

/** @return concatenation of all input strings, separated by the separator */
auto StringUtil::Join(const std::vector<std::string> &input, const std::string &separator) -> std::string {
  std::string result;

  // If the input isn't empty, append the first element. We do this so we don't need to introduce an if into the loop.
  if (!input.empty()) {
    result += input[0];
  }

  // Append the remaining input components, after the first.
  for (uint32_t i = 1; i < input.size(); i++) {
    result += separator + input[i];
  }

  return result;
}

/** @return prefix prepended to the beginning of each line in str */
auto StringUtil::Prefix(const std::string &str, const std::string &prefix) -> std::string {
  std::vector<std::string> lines = StringUtil::Split(str, '\n');

  if (lines.empty()) {
    return ("");
  }

  std::ostringstream os;
  for (uint64_t i = 0, cnt = lines.size(); i < cnt; i++) {
    if (i > 0) {
      os << std::endl;
    }
    os << prefix << lines[i];
  }
  return (os.str());
}

/** @return bytes formatted into the appropriate kilobyte, megabyte or gigabyte representation */
auto StringUtil::FormatSize(uint64_t bytes) -> std::string {
  // http://ubuntuforums.org/showpost.php?p=10215516&postcount=5
  double base = 1024;
  double kb = base;
  double mb = kb * base;
  double gb = mb * base;

  std::ostringstream os;

  if (bytes >= gb) {
    os << std::fixed << std::setprecision(2) << (bytes / gb) << " GB";
  } else if (bytes >= mb) {
    os << std::fixed << std::setprecision(2) << (bytes / mb) << " MB";
  } else if (bytes >= kb) {
    os << std::fixed << std::setprecision(2) << (bytes / kb) << " KB";
  } else {
    os << std::to_string(bytes) + " bytes";
  }
  return (os.str());
}

/** @return string wrapped with control characters to appear bold in the console */
auto StringUtil::Bold(const std::string &str) -> std::string {
  std::string set_plain_text = "\033[0;0m";
  std::string set_bold_text = "\033[0;1m";

  std::ostringstream os;
  os << set_bold_text << str << set_plain_text;
  return (os.str());
}

/** @return uppercase version of the string */
auto StringUtil::Upper(const std::string &str) -> std::string {
  std::string copy(str);
  std::transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c) { return std::toupper(c); });
  return (copy);
}

/** @return lowercase version of the string */
auto StringUtil::Lower(const std::string &str) -> std::string {
  std::string copy(str);
  std::transform(copy.begin(), copy.end(), copy.begin(), [](unsigned char c) { return std::tolower(c); });
  return (copy);
}

/** @return string formatted with printf semantics */
// NOLINTNEXTLINE - it wants us to take fmt_str as const&, but we shouldn't do that since we use it in va_args.
std::string StringUtil::Format(std::string fmt_str, ...) {
  // http://stackoverflow.com/a/8098080
  // Reserve two times as much as the length of the format string.
  int final_n;
  int n = (static_cast<int>(fmt_str.size())) * 2;
  std::string str;
  std::unique_ptr<char[]> formatted;
  va_list ap;

  while (true) {
    // Wrap the plain char array into the unique_ptr.
    formatted = std::make_unique<char[]>(n);
    strcpy(&formatted[0], fmt_str.c_str());  // NOLINT
    va_start(ap, fmt_str);
    final_n = vsnprintf(&formatted[0], static_cast<size_t>(n), fmt_str.c_str(), ap);
    va_end(ap);
    if (final_n < 0 || final_n >= n) {
      n += abs(final_n - n + 1);
    } else {
      break;
    }
  }
  return {formatted.get()};
}

/** @return input string split based on the split string */
auto StringUtil::Split(const std::string &input, const std::string &split) -> std::vector<std::string> {
  std::vector<std::string> splits;

  size_t last = 0;
  size_t input_len = input.size();
  size_t split_len = split.size();
  while (last <= input_len) {
    size_t next = input.find(split, last);
    if (next == std::string::npos) {
      next = input_len;
    }

    // Push the substring [last, next) on to splits.
    std::string substr = input.substr(last, next - last);
    if (!substr.empty()) {
      splits.push_back(substr);
    }
    last = next + split_len;
  }
  return splits;
}

/** @return count occurrence of specified string in input string */
auto StringUtil::Count(const std::string &input, const std::string &str) -> size_t {
  size_t count = 0;
  size_t n_pos = input.find(str, 0);  // first occurrence
  while (n_pos != std::string::npos) {
    count++;
    n_pos = input.find(str, n_pos + 1);
  }
  return count;
}

/**
 * Return a new string that has stripped all occurrences of the provided character from the provided string.
 *
 * NOTE: WASTEFUL. Performs a copy. Do NOT use for performance-critical code!
 *
 * @param str input string
 * @param c character to be removed
 * @return a new string with no occurrences of the provided character
 */
auto StringUtil::Strip(const std::string &str, char c) -> std::string {
  // There's a copy here which is wasteful, so don't use this in performance-critical code!
  std::string tmp = str;
  tmp.erase(std::remove(tmp.begin(), tmp.end(), c), tmp.end());
  return tmp;
}

/**
 * Replace parts of the string from `from` to `to`.
 *
 * @param source input string
 * @param from substring to be searched
 * @param to replace `from` to `to`
 * @return a new string with all occurrences of `from` replaced with `to`.
 */
auto StringUtil::Replace(std::string source, const std::string &from, const std::string &to) -> std::string {
  uint64_t start_pos = 0;
  while ((start_pos = source.find(from, start_pos)) != std::string::npos) {
    source.replace(start_pos, from.length(), to);
    start_pos += to.length();  // In case 'to' contains 'from', like
                               // replacing 'x' with 'yx'
  }
  return source;
}

/**
 * Add indention to all lines of the `lines` variable.
 *
 * @param lines input string
 * @param num_indent number of spaces prepended to each line
 * @param except_first_line if true, the first line is not indented
 * @return a new string with spaces added to each line
 */
auto StringUtil::IndentAllLines(const std::string &lines, size_t num_indent, bool except_first_line) -> std::string {
  std::vector<std::string> lines_str;
  auto lines_split = StringUtil::Split(lines, '\n');
  lines_str.reserve(lines_split.size());
  auto indent_str = StringUtil::Indent(num_indent);
  bool is_first_line = true;
  for (auto &line : lines_split) {
    if (is_first_line) {
      is_first_line = false;
      if (except_first_line) {
        lines_str.push_back(line);
        continue;
      }
    }
    lines_str.push_back(fmt::format("{}{}", indent_str, line));
  }
  return fmt::format("{}", fmt::join(lines_str, "\n"));
}

}  // namespace bustub
