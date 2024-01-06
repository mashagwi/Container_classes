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

  void reverse() noexcept {
    using std::swap;
    auto curr = head;
    while (curr != tail) {
      swap(curr->next, curr->prev);
      curr = curr->prev;
    }
    swap(tail->next, tail->prev);
    head = tail->next;
  }

  void splice(const_iterator pos, list &other) {
    auto p = pos.ptr;
    auto b = other.begin().ptr;
    auto e = other.end().ptr;
    p->prev->next = b;
    b->prev = p->prev;
    e->prev->next = p;
    p->prev = e->prev;
    head = tail->next;
    other.head = other.tail;
    other.tail->prev = other.tail->next = tail;
  }

  void unique() {
    for (auto curr = ++begin(), prev = begin(); curr != end(); curr = prev++)
      if (*curr == *prev) erase(curr);
  }
};

}  // namespace s21

#endif  // S21_LIST_H_
