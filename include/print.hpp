#pragma once

#include <fmt/base.h>

#include <list>
#include <string>
#include <vector>

namespace ccoo::print {
  template <typename T>
  void list(const std::list<T>& ll) {
    fmt::print("{{ ");
    size_t i = 0;
    for (const T& elt : ll) {
      fmt::print("{}{}", elt, i + 1 >= ll.size() ? "" : ", ");
      ++i;
    }
    fmt::println(" }}");
  }

  template <typename T>
  void vector(const std::vector<T>& vec) {
    fmt::print("{{");
    for (size_t i = 0; i < vec.size(); i++) {
      fmt::print("{}", vec[i]);
      if (i + 1 < vec.size()) fmt::print(",");
    }
    fmt::println("}}");
  }

  /**
   * Prints a given vector with a character under the provided index.
   *
   * @param vec Vector to print
   * @param index Index to draw character under
   */
  template <typename T>
  void vector_with_index(const std::vector<T>& vec, size_t index) {
    std::string vec_str = vec_to_str(vec);

    size_t substr_i = 0;
    for (size_t i = 0; i < index; i++) { substr_i = vec_str.find_first_of(",", substr_i + 1); }

    // 2 characters for '[ ' or ', '
    fmt::println("{}", vec_str);
    fmt::println("{}{}", std::string(substr_i + 2, ' '), "^");
  }

  /**
   * Prints a given vector with a character under the provided indicies.
   *
   * @param vec Vector to print
   * @param index1 First index to draw character under
   * @param index2 Second index to draw character under
   */
  template <typename T>
  void vector_with_index(const std::vector<T>& vec, size_t index1, size_t index2) {
    std::string vec_str = vec_to_str(vec);

    size_t substr_i1 = 0;
    for (size_t i = 0; i < index1; i++) { substr_i1 = vec_str.find_first_of(",", substr_i1 + 1); }

    size_t substr_i2 = 0;
    for (size_t i = 0; i < index2; i++) { substr_i2 = vec_str.find_first_of(",", substr_i2 + 1); }

    // 2 characters for '[ ' or ', '
    fmt::println("{}", vec_str);
    fmt::println("{}{}", std::string(substr_i1 + 2, ' '), "^");
    fmt::println("{}{}", std::string(substr_i2 + 2, ' '), "^");
  }

}
