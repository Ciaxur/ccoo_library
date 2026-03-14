#pragma once

#include <fmt/base.h>
#include <fmt/format.h>
#include <initializer_list>

namespace ccoo::data_structure {
  template <typename T>
  struct Node {
    public:
      T val;
      Node<T>* next = nullptr;
      Node<T>* prev = nullptr;
  };

  template <typename T>
  class LinkedList {
    private:
      Node<T>* head;
      Node<T>* tail;
      size_t _size;

    public:
      LinkedList() : head(nullptr), tail(nullptr), _size(0) {}

      LinkedList(std::initializer_list<T> entries) : head(nullptr), tail(nullptr), _size(0) { insert(entries); }

      LinkedList(LinkedList& other) : head(nullptr), tail(nullptr), _size(0) {
        if (other._size == 1) {
          head = new Node<T>(*other.head);
          tail = head;
        }
        else if (other._size == 2) {
          head = new Node<T>(*other.head);

          Node<T>* cur       = head;
          Node<T>* other_cur = other.head->next;

          while(other_cur) {
            cur->next = new Node<T>(*other_cur);
            cur       = cur->next;

            other_cur = other_cur->next;
          }

          tail = cur;
        }

        _size = other._size;
      }

      LinkedList(LinkedList&& other) {
        head  = other.head;
        tail  = other.tail;
        _size = other._size;

        other._size = 0;
        other.head  = nullptr;
        other.tail  = nullptr;
      }

      ~LinkedList() {
        Node<T>* cur = head;
        while (cur) {
          Node<T>* next = cur->next;
          delete cur;
          cur = next;
        }
      }

      void insert(std::initializer_list<T> entries) {
        for (const T& val : entries) { push_back(val); }
      }

      Node<T>* front() { return head; }
      Node<T>* back() { return tail; }

      void push_back(const T& val) {
        /* Empty LinkedList */
        if (_size == 0) {
          head      = new Node<T>();
          head->val = val;
          tail      = head;
        }

        /* Single entry within the LinkedList */
        else if (_size == 1) {
          tail       = new Node<T>();
          tail->val  = val;
          tail->prev = head;
          head->next = tail;
        }

        /* 2+ entries within the LinkedList */
        else {
          tail->next       = new Node<T>();
          tail->next->prev = tail;

          tail      = tail->next;
          tail->val = val;
        }

        ++_size;
      }

      void pop_back() {
        if (_size == 0) {
          return;
        }

        else if (_size == 1) {
          delete head;
          head = nullptr;
          tail = nullptr;
        }

        else if (_size == 2) {
          tail->prev = nullptr;
          delete tail;

          tail       = head;
          head->next = nullptr;
        }

        else {
          Node<T>* node = tail;

          tail       = node->prev;
          tail->next = nullptr;

          delete node;
        }

        --_size;
      }

      void push_front(const T& val) {
        if (_size == 0) {
          head      = new Node<T>();
          head->val = val;
          tail      = head;
        }

        else if (_size == 1) {
          Node<T>* node = new Node<T>();
          node->val     = val;

          head->prev = node;
          node->next = head;

          head = node;
        }

        else {
          Node<T>* node = new Node<T>();
          node->val     = val;

          head->prev = node;
          node->next = head;

          head = node;
        }

        ++_size;
      }

      void pop_front() {
        if (_size == 0) { return; }

        else if (_size == 1) {
          delete head;
          head = nullptr;
          tail = nullptr;
        }

        else if (_size == 2) {
          delete head;
          tail->prev = nullptr;
          head       = tail;
        }

        else {
          Node<T>* node = head;

          head       = head->next;
          head->prev = nullptr;

          delete node;
        }

        --_size;
      }

      /**
       * Removes any association/ownership between the given node and this instance. Which means
       * that the given node is now owned by the caller. So, no memory management is handled in this
       * function.
       *
       * @param node Node instance to remove from the current instance.
       */
      void erase(Node<T>* node) {
        if (_size == 0) { return; }

        else if (_size == 1) {
          head = nullptr;
          tail = nullptr;
        }

        else if (_size == 2) {
          if (node == head) {
            head       = tail;
            tail->prev = nullptr;
          }

          else {
            tail       = head;
            head->next = nullptr;
          }

          node->next = nullptr;
          node->prev = nullptr;
        }


        else {
          node->prev->next = node->next;
          node->next->prev = node->prev;

          node->prev = nullptr;
          node->next = nullptr;
        }

        _size--;
      }

      size_t size() const { return _size; }
  };

  template <typename T>
  std::string to_string(Node<T>* const head) {
    std::string str;
    str.reserve(256);

    str += fmt::format("[ ");
    Node<T>* cur = head;
    while (cur) {
      const std::string separator = cur->next == nullptr ? " " : ", ";
      str += fmt::format("{}{}", cur->val, separator);
      cur = cur->next;
    }
    str += "]";

    return str;
  }
}
