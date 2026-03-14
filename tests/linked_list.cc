#include "include/linked_list.hpp"

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <cstddef>

#include "test_suite.h"

namespace ccoo::data_structure {
  bool test_ll_push_pop() {
    LinkedList<int> ll;

    {
      for (size_t i = 0; i < 10; i++) { ll.push_back(i); }
      ASSERT_EQ(ll.size(), 10);

      for (size_t i = 0; i < 10; i++) {
        int actual_val   = ll.back()->val;
        int expected_val = 9 - i;
        ASSERT_EQ(actual_val, expected_val);
        ll.pop_back();
      }
      ASSERT_EQ(ll.size(), 0);
    }

    {
      for (size_t i = 0; i < 10; i++) { ll.push_front(i); }
      ASSERT_EQ(ll.size(), 10);

      for (size_t i = 0; i < 10; i++) {
        int actual_val   = ll.front()->val;
        int expected_val = 9 - i;
        ASSERT_EQ(actual_val, expected_val);
        ll.pop_front();
      }
      ASSERT_EQ(ll.size(), 0);
    }

    return true;
  }

  bool test_ll_raii_copy() {
    LinkedList<int> ll0;
    for (size_t i = 0; i < 10; i++) { ll0.push_back(i); }
    ASSERT_EQ(ll0.size(), 10);

    {
      LinkedList<int> ll1 = ll0;
      ASSERT_EQ(ll0.size(), ll1.size());

      Node<int>* cur0 = ll1.front();
      Node<int>* cur1 = ll1.front();

      while(cur0 && cur1) {
        ASSERT_PTR_NE(cur0, cur1);
        ASSERT_EQ(cur0->val, cur1->val);
        cur0 = cur0->next;
        cur1 = cur1->next;
      }
    }

    ASSERT_EQ(ll0.size(), 10);
    Node<int>* cur = ll0.front();
    for (int i = 0; i < 10; i++) {
      ASSERT_EQ(cur->val, i);
      cur = cur->next;
    }

    return true;
  }

  bool test_ll_raii_move() {
    LinkedList<int> ll0;
    for (size_t i = 0; i < 10; i++) { ll0.push_back(i); }
    ASSERT_EQ(ll0.size(), 10);

    {
      LinkedList<int> ll1 = std::move(ll0);
      ASSERT_EQ(ll0.size(), 0);
      ASSERT_EQ(ll1.size(), 10);

      Node<int>* cur = ll1.front();
      for (int i = 0; i < 10; i++) {
        ASSERT_EQ(cur->val, i);
        cur = cur->next;
      }
    }

    return true;
  }

  bool test_ll_raii_pass_by_ref() {
    const auto fn = [](LinkedList<int>& ll_ref) {
      ASSERT_EQ(ll_ref.size(), 10);

      Node<int>* cur = ll_ref.front();
      for (int i = 0; i < 10; i++) {
        ASSERT_EQ(cur->val, i);
        cur = cur->next;
      }

      return true;
    };

    LinkedList<int> ll0;
    for (size_t i = 0; i < 10; i++) { ll0.push_back(i); }

    ASSERT_EQ(fn(ll0), true);
    ASSERT_EQ(ll0.size(), 10);

    Node<int>* cur = ll0.front();
    for (int i = 0; i < 10; i++) {
      ASSERT_EQ(cur->val, i);
      cur = cur->next;
    }

    return true;
  }

  bool test_ll_erase() {
    {
      LinkedList<int> ll;
      ll.push_back(0);

      Node<int>* node = ll.front();
      ll.erase(node);

      ASSERT_PTR_EQ(node->next, nullptr);
      ASSERT_PTR_EQ(node->prev, nullptr);
      delete node;

      ASSERT_EQ(ll.size(), 0);

      ASSERT_PTR_EQ(ll.front(), nullptr);
      ASSERT_PTR_EQ(ll.back(), nullptr);
    }

    {
      LinkedList<int> ll;
      ll.push_back(0);
      ll.push_back(1);

      Node<int>* node = ll.front();
      ll.erase(node);

      ASSERT_PTR_EQ(node->next, nullptr);
      ASSERT_PTR_EQ(node->prev, nullptr);
      delete node;

      ASSERT_EQ(ll.size(), 1);

      ASSERT_PTR_NE(ll.front(), nullptr);
      ASSERT_PTR_NE(ll.back(), nullptr);

      ASSERT_EQ(ll.front()->val, 1);
      ASSERT_EQ(ll.back()->val, 1);
    }

    {
      LinkedList<int> ll;
      ll.push_back(0);
      ll.push_back(1);

      Node<int>* node = ll.front()->next;
      ll.erase(node);

      ASSERT_PTR_EQ(node->next, nullptr);
      ASSERT_PTR_EQ(node->prev, nullptr);
      delete node;

      ASSERT_EQ(ll.size(), 1);

      ASSERT_PTR_NE(ll.front(), nullptr);
      ASSERT_PTR_NE(ll.back(), nullptr);

      ASSERT_EQ(ll.front()->val, 0);
      ASSERT_EQ(ll.back()->val, 0);
    }

    {
      LinkedList<int> ll;
      ll.push_back(0);
      ll.push_back(1);
      ll.push_back(2);

      Node<int>* node = ll.front()->next;
      ll.erase(node);

      ASSERT_PTR_EQ(node->next, nullptr);
      ASSERT_PTR_EQ(node->prev, nullptr);
      delete node;

      ASSERT_EQ(ll.size(), 2);

      node = ll.front();
      ASSERT_EQ(node->val, 0);
      ASSERT_EQ(node->next->val, 2);
      ASSERT_PTR_EQ(node->next->next, nullptr);

      ASSERT_PTR_NE(ll.front(), nullptr);
      ASSERT_PTR_NE(ll.back(), nullptr);
    }

    return true;
  }

  bool test_ll_initializer_list() {
    {
      LinkedList<int> ll;

      ll.insert({ 1, 2, 3, 4 });
      ASSERT_EQ(ll.size(), 4);

      Node<int>* cur = ll.front();
      for (int i = 1; i < 5; i++) {
        ASSERT_EQ(cur->val, i);
        cur = cur->next;
      }
    }

    {
      LinkedList<int> ll({ 1, 2, 3, 4 });
      ASSERT_EQ(ll.size(), 4);

      Node<int>* cur = ll.front();
      for (int i = 1; i < 5; i++) {
        ASSERT_EQ(cur->val, i);
        cur = cur->next;
      }
    }

    return true;
  }
}


int main() {
  using namespace ccoo::data_structure;
  bool success = true;

  success = success && RUN_TEST(test_ll_push_pop);
  success = success && RUN_TEST(test_ll_raii_copy);
  success = success && RUN_TEST(test_ll_raii_move);
  success = success && RUN_TEST(test_ll_raii_pass_by_ref);
  success = success && RUN_TEST(test_ll_erase);
  success = success && RUN_TEST(test_ll_initializer_list);

  return success ? 0 : 1;
}
