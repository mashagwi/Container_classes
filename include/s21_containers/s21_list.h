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
  size_type size_{0};

 public:
  template <typename value_type>
  struct ListConstIterator;
  template <typename value_type>
  struct ListIterator {
    ListIterator(Node *p = nullptr) : ptr(p) {}

    reference operator*() const { return ptr->data; }

    pointer operator->() const { return &**this; }

    ListIterator &operator++() {
      ptr = ptr->next;
      return *this;
    }

    ListIterator operator++(int) {
      auto ret = *this;
      ++*this;
      return ret;
    }

    ListIterator &operator--() {
      ptr = ptr->prev;
      return *this;
    }

    ListIterator operator--(int) {
      auto ret = *this;
      --*this;
      return ret;
    }

    bool operator==(const ListIterator &other) const {
      return ptr == other.ptr;
    }

    bool operator!=(const ListIterator &other) const {
      return !(*this == other);
    }

    operator ListConstIterator<value_type>() const {
      return ListConstIterator<value_type>(ptr);
    }

   private:
    friend class list<value_type>;
    friend class ListConstIterator<value_type>;
    Node *ptr;
  };

  template <typename value_type>
  struct ListConstIterator : public ListIterator<value_type> {
    using ListIterator<value_type>::ListIterator;
    const_reference operator*() const {
      return ListIterator<value_type>::operator*();
    }

    const_pointer operator->() const {
      return ListIterator<value_type>::operator->();
    }

   private:
    friend class list<value_type>;
  };

  using iterator = ListIterator<T>;
  using const_iterator = ListConstIterator<T>;

  iterator begin() noexcept { return iterator(head); }

  iterator end() noexcept { return iterator(tail); }

  const_iterator begin() const noexcept { return const_iterator(head); }

  const_iterator end() const noexcept { return const_iterator(tail); }

  const_iterator cbegin() const noexcept { return begin(); }

  const_iterator cend() const noexcept { return end(); }

  list() {
    tail = new Node();
    tail->prev = tail->next = tail;
    head = tail;
  }

  explicit list(size_type count) : list() {
    while (count--) push_back(value_type());
  }

  list(std::initializer_list<value_type> const &items) : list() {
    for (const auto &e : items) push_back(e);
  }

  list(const list &other) : list() {
    for (const auto &e : other) push_back(e);
  }

  list(list &&other) noexcept
      : head(other.head), tail(other.tail), size_(other.size_) {
    other.head = other.tail = nullptr;
    other.size_ = 0;
  }

  list &operator=(list &&other) noexcept {
    if (this != &other) {
      free();
      head = other.head;
      tail = other.tail;
      size_ = other.size_;
      other.head = other.tail = nullptr;
      other.size_ = 0;
    }
    return *this;
  }

  ~list() { free(); }

  void free() {
    clear();
    delete tail;
  }

  reference front() { return *begin(); }

  reference back() { return *--end(); }

  const_reference front() const { return *begin(); }

  const_reference back() const { return *--end(); }

  bool empty() const noexcept { return size_ == 0; }

  size_type size() const noexcept { return size_; }

  size_type max_size() const noexcept { return SIZE_MAX / sizeof(Node) / 2; }

  void clear() noexcept {
    while (head != tail) pop_back();
  }

  iterator insert(const_iterator pos, const_reference value) {
    Node *n = new Node(value);
    n->next = pos.ptr;
    n->prev = pos.ptr->prev;
    n->prev->next = pos.ptr->prev = n;
    if (head == pos.ptr) head = n;
    ++size_;
    return n;
  }

  iterator erase(const_iterator pos) {
    auto popped = pos.ptr;
    auto ret = popped->next;
    popped->prev->next = popped->next;
    popped->next->prev = popped->prev;
    --size_;
    if (head == popped) head = popped->next;
    ;
    delete popped;
    return ret;
  }

  void push_back(const_reference value) {
    insert(cend(), value);
    //    Node *n = new Node(value);
    //    n->next = tail;
    //    n->prev = tail->prev;
    //    n->prev->next = tail->prev = n;
    //    if (empty()) head = n;
    //    ++size_;
  }

  void pop_back() {
    erase(--cend());
    //		tail->prev->next = fake_end;
    //    fake_end->prev = tail->prev;
    //    delete tail;
    //    tail = fake_end->prev;
    //    --size_;
    //    if (empty()) head = fake_end;
  }

  void push_front(const_reference value) { insert(cbegin(), value); }

  void pop_front() { erase(cbegin()); }

  void swap(list &other) noexcept {
    using std::swap;
    swap(head, other.head);
    swap(tail, other.tail);
    swap(size_, other.size_);
  }

  // TODO: segmentation with even number of elements
  void reverse() noexcept {
    using std::swap;
    auto b = begin();
    auto e = --end();
    swap(tail->next, tail->prev);
    head = e.ptr;
    for (size_type i = 0; i != size() / 2; ++i) {
      std::swap(b.ptr->next, e.ptr->next);
      std::swap(b.ptr->prev, e.ptr->prev);
      b = e.ptr->next;
      e = b.ptr->prev;
    }
    if (size() % 2 == 1) std::swap(b.ptr->next, b.ptr->prev);
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
