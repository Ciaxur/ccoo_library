#pragma once

#include <fmt/base.h>
#include <fmt/format.h>

#include <string>
#include <vector>

namespace ccoo::stringify {
  template <typename T>
  std::string vector(const std::vector<T>& vec) {
    std::string str;
    str.reserve(vec.size() * 2);

    str += fmt::format("[ ");
    for (size_t i = 0; i < vec.size(); i++) {
      const std::string separator = i + 1 == vec.size() ? " " : ", ";
      str += fmt::format("{}{}", vec[i], separator);
    }
    str += "]";

    return str;
  }

  template <typename T>
  std::string vector2d(const std::vector<std::vector<T>>& vec2d, const char delim = '\n') {
    std::string str;
    str.reserve(10 + (vec2d.size() * (vec2d.empty() ? 1 : vec2d.size())));

    for (size_t i = 0; i < vec2d.size(); i++) {
      const std::vector<T>& vec = vec2d[i];
      str += vec_to_str(vec);

      if (i + 1 < vec2d.size()) { str += delim; }
    }

    return str;
  }
}
