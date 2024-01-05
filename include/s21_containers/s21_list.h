#ifndef S21_LIST_H_
#define S21_LIST_H_

#include <cstddef>
#include <initializer_list>
#include <memory>
#include <utility>

namespace s21 {

template <typename T>
class list {
 public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using size_type = std::size_t;
  using pointer = T *;
  using const_pointer = const T *;

 private:
  struct Node {
    Node() = default;
    Node(const_reference value) : data(value) {}

    Node *next{nullptr};
    Node *prev{nullptr};
    value_type data;
  };

  Node *head{nullptr};
  Node *tail{nullptr};
  Node *fake_end{nullptr};
  size_type size_{0};

 public:
  template <typename value_type>
  struct Iterator {
    Iterator(Node *p = nullptr) : ptr(p) {}

    reference operator*() const { return ptr->data; }

    pointer operator->() const { return &**this; }

    Iterator &operator++() {
      ptr = ptr->next;
      return *this;
    }

    Iterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    Iterator &operator--() {
      ptr = ptr->prev;
      return *this;
    }

    Iterator operator--(int) {
      auto ret = *this;
      --*this;
      return ret;
    }

    bool operator==(const Iterator &rhs) const { return ptr == rhs.ptr; }

    bool operator!=(const Iterator &rhs) const { return !(*this == rhs); }

   private:
    friend class list<value_type>;
    Node *ptr;
  };

  template <typename value_type>
  struct ConstIterator : public Iterator<value_type> {
	  using Iterator<value_type>::Iterator;
    const_reference operator*() const {
      return Iterator<value_type>::operator*();
    }

    const_pointer operator->() const {
      return Iterator<value_type>::operator->();
    }

   private:
    friend class list<value_type>;
  };

  using iterator = Iterator<T>;
  using const_iterator = ConstIterator<T>;

  iterator begin() const { return Iterator<T>(head); }

  iterator end() const { return Iterator<T>(fake_end); }

  const_iterator cbegin() const { return ConstIterator<T>(head); }

  const_iterator cend() const { return ConstIterator<T>(fake_end); }

  list() {
    fake_end = new Node();
    check_links();
  }

  ~list() { free(); }

  void check_links() {
    // if (!tail) {
    tail = head = fake_end;
    fake_end->next = fake_end->prev = fake_end;
    //}
  }

  void free() {
    while (head != fake_end) pop_back();
    delete fake_end;
  }

 public:
  bool empty() const { return size_ == 0; }

  size_type size() { return size_; }

  void push_back(const_reference value) {
    Node *n = new Node(value);
    if (empty()) head = n;
    n->next = fake_end;
    n->prev = tail;
    tail->next = n;
    fake_end->prev = tail = n;
    ++size_;
  }

  void pop_back() {
    tail->prev->next = fake_end;
    fake_end->prev = tail->prev;
    delete tail;
    tail = fake_end->prev;
    --size_;
    if (empty()) head = fake_end;
  }
};

}  // namespace s21

/* public:
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
}

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

*/

#endif  // S21_LIST_H_
