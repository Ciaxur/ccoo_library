#include "include/str_util.hpp"

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <cstring>
#include <string_view>

#include "test_suite.h"

namespace ccoo::str {
  bool test_split() {
    {
      const std::vector<std::string> sp          = split("a,b,c,d,e,f,g", ",");
      const std::vector<std::string> expected_sp = { "a", "b", "c", "d", "e", "f", "g" };
      ASSERT_EQ(sp.size(), expected_sp.size());

      for (size_t i = 0; i < expected_sp.size(); i++) { ASSERT_EQ(expected_sp[i], sp[i]); }
    }

    {
      const std::vector<std::string> sp          = split("bob,and,alice", ",");
      const std::vector<std::string> expected_sp = { "bob", "and", "alice" };
      ASSERT_EQ(sp.size(), expected_sp.size());

      for (size_t i = 0; i < expected_sp.size(); i++) { ASSERT_EQ(expected_sp[i], sp[i]); }
    }

    {
      const std::vector<std::string> sp          = split("bob,and,alice", ",and,");
      const std::vector<std::string> expected_sp = { "bob", "alice" };
      ASSERT_EQ(sp.size(), expected_sp.size());

      for (size_t i = 0; i < expected_sp.size(); i++) { ASSERT_EQ(expected_sp[i], sp[i]); }
    }

    {
      const std::vector<std::string> sp          = split("bob", "doesn't exist");
      const std::vector<std::string> expected_sp = { "bob" };
      ASSERT_EQ(sp.size(), expected_sp.size());

      for (size_t i = 0; i < expected_sp.size(); i++) { ASSERT_EQ(expected_sp[i], sp[i]); }
    }

    {
      const std::vector<std::string> sp          = split("", "");
      const std::vector<std::string> expected_sp = {};
      ASSERT_EQ(sp.size(), expected_sp.size());
    }

    {
      const std::vector<std::string> sp          = split("", "doesn't exist");
      const std::vector<std::string> expected_sp = {};
      ASSERT_EQ(sp.size(), expected_sp.size());
    }

    return true;
  }

  bool test_slice() {
    {
      const std::string s           = "Hello world";
      const std::string_view sv     = slice(s, 0, 5);

      ASSERT_PTR_EQ(s.data(), sv.data());
      ASSERT_EQ(sv.size(), 5);
      ASSERT_EQ(sv, "Hello");
    }

    {
      const std::string s       = "Hello world";
      const std::string_view sv = slice(s, 6, 5);

      ASSERT_PTR_EQ(s.data() + 6, sv.data());
      ASSERT_EQ(sv.size(), 5);
      ASSERT_EQ(sv, "world");
    }

    return true;
  }
}

int main() {
  using namespace ccoo::str;
  bool success = true;

  success = success && RUN_TEST(test_split);
  success = success && RUN_TEST(test_slice);

  return success ? 0 : 1;
};

