#include "include/threading.hpp"

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <atomic>

#include "test_suite.h"

namespace ccoo::threading {
  bool test_tasks_complete_before_destructing() {
    const constexpr size_t NUM_TASKS   = 500'000;
    const constexpr size_t NUM_WORKERS = 1;

    std::atomic<size_t> counter = 0;
    {
      ThreadPool tp(NUM_WORKERS);
      ASSERT_EQ(tp.size(), NUM_WORKERS)

      for (size_t i = 0; i < NUM_TASKS; i++) {
        tp.add_task([&]() { ++counter; });
      }
    }
    ASSERT_EQ((size_t)counter, NUM_TASKS);

    return true;
  }

  bool test_all_tasks_complete() {
    const constexpr size_t NUM_TASKS   = 500'000;
    const constexpr size_t NUM_WORKERS = 100;

    std::atomic<size_t> counter = 0;
    {
      ThreadPool tp(NUM_WORKERS);
      ASSERT_EQ(tp.size(), NUM_WORKERS)

      for (size_t i = 0; i < NUM_TASKS; i++) {
        tp.add_task([&]() { ++counter; });
      }
    }
    ASSERT_EQ((size_t)counter, NUM_TASKS);

    return true;
  }

  bool test_empty_tasks() {
    ThreadPool tp(5);
    return true;
  }
}

int main() {
  using namespace ccoo::threading;
  bool success = true;

  success = success && RUN_TEST(test_tasks_complete_before_destructing);
  success = success && RUN_TEST(test_all_tasks_complete);
  success = success && RUN_TEST(test_empty_tasks);

  return success ? 0 : 1;
};
