#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace ccoo::str {
  /**
   * Similar to higher-level languages' split function, where this splits a given string by the
   * given delimeter.
   *
   * @param s String reference to split
   * @param delim Delimeter to use for splitting
   *
   * @returns Vector of split strings
   */
  std::vector<std::string> split(const std::string& s, const char* delim) {
    std::vector<std::string> entries;
    if (s.empty()) { return entries; }

    size_t i = 0;
    size_t j = s.find(delim, i);

    while (j != s.npos && i != j) {
      entries.emplace_back(s.substr(i, j - i));
      i = j + 1;
      j = s.find(delim, i);
    }
    entries.emplace_back(s.substr(i));

    return entries;
  }

  /**
   * Thin-wrapper mimicking std::string::substr, but returns a string_view instead of a newly
   * constructed string.
   *
   * @param str String reference to slice
   * @param pos Starting position from which to slice
   * @param stride Number of characters to slice, starting from position
   *
   * @returns string_view slice instance
   */
  std::string_view slice(const std::string& str, size_t pos, size_t stride) {
    return std::string_view(str.data() + pos, stride);
  }
};
