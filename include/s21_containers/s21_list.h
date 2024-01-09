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
    if (!other.empty()) {
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
      size_ += other.size();
      other.size_ = 0;
    }
  }

  void merge(list &other) {
    if (this == &other) return;
    auto curr = head;
    auto other_curr = other.head;
    auto temp = curr;
    while (other_curr != other.tail) {
      if (curr == tail) {
        splice(end(), other);
        return;
      }
      if (!(curr->data < other_curr->data)) {
        temp = other_curr->next;
        temp->prev = other.tail;
        curr->prev->next = other_curr;
        other_curr->prev = curr->prev;
        curr->prev = other_curr;
        other_curr->next = curr;
        if (curr == head) head = other_curr;
        other_curr = temp;
      } else
        curr = curr->next;
    }
    other.head = other.tail;
    other.tail->prev = other.tail->next = tail;
    size_ += other.size();
    other.size_ = 0;
  }

  void unique() {
    for (auto curr = begin(), prev = begin(); curr != end(); curr = prev)
      if (*++curr == *prev)
        erase(curr);
      else
        ++prev;
  }

  void sort() {
    if (size() > 1) {
	    QuickSort(1, size());
	    head = tail->next;
    }
  }

  void QuickSort(size_type first, size_type last) {
    using std::swap;
		////		 Node* partition(Node *l, Node *h)
		////	{
		////			// set h element as pivot
		////			int x = h->data;
		////			Node *i = l->prev_node;

		////			// Similar to from j=l to h-1
		////			for (Node *j = l; j != h; j = j->next_node)
		////			{
		////					if (j->data <= x)
		////					{
		////							// Similar to i++
		////							i = (i == NULL)? l : i->next_node;
		////							swap(&(i->data), &(j->data));
		////					}
		////			}

		////			i = (i == NULL)? l : i->next_node;
		////			swap(&(i->data), &(h->data));
		////			return i;
		////	}

		////	/* Recursive implementation of quicksort for linked list */
		////	void _quickSort(Node* l, Node *h)
		////	{
		////			if (h != NULL && l != h && l != h->next_node)
		////			{
		////					Node *p = partition(l, h);
		////					_quickSort(l, p->prev_node);
		////					_quickSort(p->next_node, h);
		////			}

		////	}

		////	// Main function: It calls the _quickSort() method

		////	void quickSort(Node *head)
		////	{
		////			// Find the last node
		////			Node *h = lastNode(head);
		////			// Call recursive QuickSort
		////			_quickSort(head, h);
		////	} 
		
		//if (first < last) {
    //  size_type l = first, r = last, n = 0;

    //  Node *left = tail->next;
    //  while (++n < l) left = left->next;

    //  Node *mid = left;
    //  while (n++ < (l + r) / 2) mid = mid->next;

    //  Node *right = mid;
    //  while (n++ <= r) right = right->next;

    //  // Node *temp_first = left->prev;
    //  // Node *temp_last = right->next;

    //  do {
    //    while (left->next != tail && left->data < mid->data) {
    //      left = left->next;
    //      ++l;
    //    }
    //    while (right->prev != tail && mid->data < right->data) {
    //      right = right->prev;
    //      --r;
    //    }
    //    if (l <= r) {
		//			left->prev->next = right;
		//			right->next->prev = left;
		//			if (left != right->prev) {
		//				left->next->prev = right;
		//				right->prev->next = left;
		//				swap(left->next, right->next);
		//				swap(left->prev, right->prev);
		//			} else {
		//				swap(left->next, left->prev);
		//				swap(right->next, right->prev);
		//				swap(left->next, right->prev);
		//			}
    //      l++;
    //      r--;
    //    }
    //  } while (l <= r);
    //  //} while (l <= r);
    //  QuickSort(first, r);
    //  QuickSort(l, last);
    //}
  }

  // Node *mid = head;
  // size_type n = 0, f = 1, l = size();
  // while (++n < l / 2)
  //	mid = mid->next;
  // do {
  //	while (f->next != tail && f->data < mid->data) {
  //		first = first->next;
  //	while (l->prev != tail && (mid->data < l->data))
  //		l = l->prev;
  //	if (f != l) {
  //		swap(f->prev->next, l->prev->next);
  //		swap(f->next->prev, l->next->prev);
  //		swap(f->prev, l->prev);
  //		swap(f->next, l->next);
  //		f++;
  //		l--;
  //	}
  //} while (f != l);
  // if (first != l) QuickSort(first, l);
  // if (f != last) QuickSort(f, last);
};

}  // namespace s21

#endif  // S21_LIST_H_
