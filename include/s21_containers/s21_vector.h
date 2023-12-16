#ifndef S21_VECTOR_H_
#define S21_VECTOR_H_

#include <cstddef>
#include <initializer_list>
#include <utility>

namespace s21 {

// Test vector class with some basic example operations and concepts
template <class T>
class vector {
 private:
  size_t m_size;
  size_t m_capacity;
  T *arr;

 public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using iterator = T *;
  using const_iterator = const T *;
  using size_type = size_t;

 private:
  void reserve_more_capacity(size_type size);

 public:
  // default constructor
  vector() : m_size(0U), m_capacity(0U), arr(nullptr) {}
  // parametrized constructor for fixed size vector (explicit was used in order
  // to avoid automatic type conversion)
  explicit vector(size_type n)
      : m_size(n), m_capacity(n), arr(n ? new T[n] : nullptr) {}
  // initializer list constructor
  vector(std::initializer_list<value_type> const &items);
  // copy constructor with simplified syntax
  vector(const vector &v)
      : m_size(v.m_size), m_capacity(v.m_capacity), arr(v.arr){};
  // move constructor with simplified syntax
  vector(vector &&v) : m_size(v.m_size), m_capacity(v.m_capacity), arr(v.arr) {
    v.arr = nullptr;
    v.m_size = 0;
  }

  ~vector() { delete[] arr; }

  // some method examples
  // size getter
  size_type size();
  // element accessor
  value_type at(size_type i);
  // append new element
  void push_back(value_type v);
};

template <typename T>
void vector<T>::reserve_more_capacity(size_t size) {
  if (size > m_capacity) {
    value_type *buff = new value_type[size];
    for (size_t i = 0; i < m_size; ++i) buff[i] = std::move(arr[i]);
    delete[] arr;
    arr = buff;
    m_capacity = size;
  }
}

template <typename T>
vector<T>::vector(std::initializer_list<value_type> const &items) {
  arr = new value_type[items.size()];
  int i = 0;
  for (auto it = items.begin(); it != items.end(); it++) {
    arr[i] = *it;
    i++;
  }
  m_size = items.size();
  m_capacity = items.size();
};

template <typename T>
size_t vector<T>::size() {
  return m_size;
}

template <typename T>
T vector<T>::at(size_type i) {
  return arr[i];
}

template <typename T>
void vector<T>::push_back(T v) {
  if (m_size == m_capacity) {
    reserve_more_capacity(m_size * 2);
  }
  arr[m_size++] = v;
}

}  // namespace s21

#endif  // S21_VECTOR_H_
