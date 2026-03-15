#pragma once

#include <fmt/base.h>
#define RUN_TEST(test) evaluate_test_result((#test), (test)())

#define ASSERT_PTR_NE(v0, v1)                                                                                      \
  if ((v0) == (v1)) {                                                                                              \
    fmt::println(                                                                                                  \
      "{}:{} | FAIL: {:p} != {:p}", __FILE__, __LINE__, static_cast<const void*>(v0), static_cast<const void*>(v1) \
    );                                                                                                             \
    return false;                                                                                                  \
  }

#define ASSERT_PTR_EQ(v0, v1)                                                                                      \
  if ((v0) != (v1)) {                                                                                              \
    fmt::println(                                                                                                  \
      "{}:{} | FAIL: {:p} == {:p}", __FILE__, __LINE__, static_cast<const void*>(v0), static_cast<const void*>(v1) \
    );                                                                                                             \
    return false;                                                                                                  \
  }

#define ASSERT_NE(v0, v1)                                                   \
  if ((v0) == (v1)) {                                                       \
    fmt::println("{}:{} | FAIL: {} != {}", __FILE__, __LINE__, (v0), (v1)); \
    return false;                                                           \
  }

#define ASSERT_EQ(v0, v1)                                                   \
  if ((v0) != (v1)) {                                                       \
    fmt::println("{}:{} | FAIL: {} == {}", __FILE__, __LINE__, (v0), (v1)); \
    return false;                                                           \
  }

inline bool evaluate_test_result(const char* test_name, bool result) {
  bool is_pass = result;
  fmt::println("=== TEST '{}' [{}] ===", test_name, is_pass ? "PASS" : "FAIL");
  return is_pass;
}
