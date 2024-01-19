#ifndef RBTREE_H_
#define RBTREE_H_

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace s21 {

template <class Key, class Value, class Compare>
class RBTree;  // class pre-declaration for friend functions

template <class K, class V, class C>  // fixes non-template type fn detection
std::ostream& operator<<(std::ostream& os, const RBTree<K, V, C>& t) noexcept;

template <class Key, class Value = void, class Compare = std::less<Key>>
class RBTree {
 public:
  template <bool is_const>
  class Iterator;

  using key_type = Key;
  using mapped_type = Value;
  using value_type = std::conditional_t<std::is_void_v<Value>, Key,
                                        std::pair<const Key, Value>>;
  using size_type = size_t;
  using key_compare = Compare;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;

  RBTree() = default;
  RBTree(std::initializer_list<value_type> const& items);
  RBTree(const RBTree& other) noexcept(false);
  RBTree& operator=(const RBTree& other) noexcept(false);
  RBTree(RBTree&& other) noexcept;
  RBTree& operator=(RBTree&& other) noexcept;
  ~RBTree();

  iterator begin() const;
  iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  bool empty() const noexcept;
  size_type size() const noexcept;
  size_type max_size() const;

  void clear() noexcept;
  void erase(const Key& key);
  void erase(iterator pos);
  void swap(RBTree& other) noexcept;
  friend void swap(const RBTree& a, const RBTree& b) noexcept { a.swap(b); }
  bool contains(const Key& key) const;

  template <typename = std::enable_if<!std::is_void_v<Value>>>
  std::add_lvalue_reference_t<mapped_type> at(const Key& key) {
    iterator pos = Find(key);
    if (pos == end()) throw std::out_of_range("The entry does not exist.");
    return (*pos).second;
  }

  template <typename = std::enable_if<!std::is_void_v<Value>>>
  std::add_lvalue_reference_t<mapped_type> operator[](const Key& key) noexcept {
    iterator pos = Find(key);
    if (pos == end()) pos = Insert(std::pair(key, mapped_type{}));
    return (*pos).second;
  }

 protected:
  class Node;

 public:
  template <bool is_const>
  class Iterator {
    friend class RBTree;
    friend class Iterator<!is_const>;

   public:
    using difference_type = std::ptrdiff_t;
    using value_type = RBTree::value_type;
    using pointer =
        std::conditional_t<is_const, const value_type*, value_type*>;
    using reference =
        std::conditional_t<is_const, const value_type&, value_type&>;
    using iterator_category = std::bidirectional_iterator_tag;

    reference operator*() const;
    auto& operator++();
    auto operator++(int);
    // auto& operator--();
    // auto operator--(int);
    template <bool R>  // allow comparisons between iterator and const_iterator
    bool operator==(const Iterator<R>& rhs) const noexcept;
    template <bool R>
    bool operator!=(const Iterator<R>& rhs) const noexcept;
    operator Iterator<true>() const noexcept;  // allow conversion to const ver

   private:
    using node_pointer = std::conditional_t<is_const, const Node*, Node*>;
    explicit Iterator(node_pointer p);

    node_pointer ptr_;
  };

 protected:
  static key_type get_key(const value_type& data);
  iterator Find(const Key& key) const;
  iterator Insert(const value_type& data);
  std::pair<iterator, bool> InsertIfNe(const value_type& data);
  void InsertFixup(Node* node);
  Node* TrinodeRestructure(Node* node);
  void TransplantSubtree(Node* dst, Node* src);
  void EraseFixup(Node* node);
  void Merge(RBTree& other);
  void MergeIfNe(RBTree& other);
  template <class... Args>
  std::vector<std::pair<iterator, bool>> InsertMany(Args&&... args);
  template <class... Args>
  std::vector<std::pair<iterator, bool>> InsertManyIfNe(Args&&... args);
  Node* CopyTreeRecursive(Node* node, Node* parent, Node* other_nil);
  void DestroyRecursive(Node* node) noexcept;
  void Print(std::ostream& os) const noexcept;
  void PrintRecursive(std::ostream& os, const std::string& prefix, Node* node,
                      bool is_left) const noexcept;
  friend std::ostream& operator<< <>(std::ostream& os,
                                     const RBTree& t) noexcept;
  // mapped_type& operator[](const Key& key);

  struct Node {
    friend class RBTree;
    enum Color { red, black };

    explicit Node(const value_type& v, Color c);
    explicit Node(const value_type& v, Color c, Node* p);

    bool is_internal() const noexcept;
    bool is_external() const noexcept;
    bool is_left() const noexcept;
    bool is_right() const noexcept;
    Node* sibling() const noexcept;
    Node* parent() const noexcept;
    Node* grandparent() const noexcept;
    bool is_black() const noexcept;
    bool is_red() const noexcept;
    void set_black() noexcept;
    void set_red() noexcept;
    void set_color(Color col) noexcept;

    value_type data_;
    Color color_;
    Node* parent_ = nullptr;
    Node* left_ = nullptr;
    Node* right_ = nullptr;
  };

 private:
  Node* nil_ = new Node(value_type{}, Node::Color::black);  // sentinel node
  Node* root_ = nil_;
  size_type n_ = 0;  // true nodes (hold data provided by user)
};

template <class Key, class Value, class Compare>
RBTree<Key, Value, Compare>::RBTree(
    std::initializer_list<value_type> const& items) {
  for (auto&& elem : items) {
    Insert(elem);
  }
}

template <class Key, class Value, class Compare>
RBTree<Key, Value, Compare>::RBTree(const RBTree& other) {
  root_ = CopyTreeRecursive(other.root_, nil_, other.nil_);
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::Node*
RBTree<Key, Value, Compare>::CopyTreeRecursive(Node* node, Node* parent,
                                               Node* other_nil) {
  if (node == other_nil) return nil_;
  Node* new_node = new Node(node->data_, node->color_, parent);
  new_node->left_ = CopyTreeRecursive(node->left_, new_node, other_nil);
  new_node->right_ = CopyTreeRecursive(node->right_, new_node, other_nil);
  n_++;
  return new_node;
}

template <class Key, class Value, class Compare>
RBTree<Key, Value, Compare>::RBTree(RBTree&& other) noexcept {
  root_ = std::exchange(other.root_, nullptr);
  nil_ = std::exchange(other.nil_, nullptr);
  n_ = std::exchange(other.n_, 0);
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::RBTree&
RBTree<Key, Value, Compare>::operator=(const RBTree& other) noexcept(false) {
  RBTree<Key, Value, Compare> copy(other);
  copy.swap(*this);
  return *this;
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::RBTree&
RBTree<Key, Value, Compare>::operator=(RBTree&& other) noexcept {
  RBTree<Key, Value, Compare> copy(std::move(other));
  copy.swap(*this);
  return *this;
}

template <class Key, class Value, class Compare>
RBTree<Key, Value, Compare>::~RBTree() {
  if (n_) clear();
  delete nil_;
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::clear() noexcept {
  DestroyRecursive(root_);
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::DestroyRecursive(Node* node) noexcept {
  if (!node || node == nil_) return;
  DestroyRecursive(node->left_);
  DestroyRecursive(node->right_);
  delete node, n_--;
}

template <class Key, class Value, class Compare>
bool RBTree<Key, Value, Compare>::empty() const noexcept {
  return n_ == 0;
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::size_type
RBTree<Key, Value, Compare>::size() const noexcept {
  return n_;
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::size_type
RBTree<Key, Value, Compare>::max_size() const {
  return std::numeric_limits<typename iterator::difference_type>::max();
}

template <class Key, class Value, class Compare>
bool RBTree<Key, Value, Compare>::contains(const Key& key) const {
  auto pos = Find(key);
  return pos != end();
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::iterator
RBTree<Key, Value, Compare>::Find(const Key& key) const {
  if (root_ == nil_) return end();

  Node* cur = root_;

  key_compare cmp;
  while (cur != nil_) {
    key_type cur_key = get_key(cur->data_);
    if (key == cur_key) break;
    if (cmp(key, cur_key))
      cur = cur->left_;
    else
      cur = cur->right_;
  }

  return iterator(cur);
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::key_type
RBTree<Key, Value, Compare>::get_key(const value_type& data) {
  if constexpr (std::is_void_v<Value>)
    return data;
  else
    return data.first;
}

template <class Key, class Value, class Compare>
std::pair<typename RBTree<Key, Value, Compare>::iterator, bool>
RBTree<Key, Value, Compare>::InsertIfNe(const value_type& data) {
  if (contains(get_key(data))) return {end(), false};
  return {Insert(data), true};
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::iterator
RBTree<Key, Value, Compare>::Insert(const value_type& data) {
  if (root_ == nil_) {
    root_ = new Node(data, Node::Color::black), n_++;
    root_->parent_ = root_->left_ = root_->right_ = nil_;
    return iterator(root_);
  }

  Node* cur = root_;
  Node* par = nil_;
  key_compare cmp;
  bool is_smaller;

  while (cur != nil_) {
    par = cur;
    is_smaller = cmp(get_key(data), get_key(cur->data_));
    if (is_smaller)
      cur = cur->left_;
    else
      cur = cur->right_;
  }

  Node* node = new Node(data, Node::Color::red);
  n_++;
  node->left_ = node->right_ = nil_;
  node->parent_ = par;
  if (is_smaller)
    par->left_ = node;
  else
    par->right_ = node;

  InsertFixup(node);
  return iterator(node);
}

template <class K, class V, class C>
void RBTree<K, V, C>::InsertFixup(Node* node) {
  Node* p = node->parent_;
  if (p == root_ || p->is_black()) return;
  if (p->sibling()->is_black()) {
    p = TrinodeRestructure(node);
    p->set_black();
    p->left_->set_red();
    p->right_->set_red();
  } else {
    p->set_black();
    p->sibling()->set_black();
    Node* g = p->parent_;
    if (g == root_) return;
    g->set_red();
    InsertFixup(g);
  }
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::erase(const Key& key) {
  iterator pos = Find(key);
  if (pos != end()) erase(pos);
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::erase(iterator pos) {
  Node* u = pos.ptr_;
  Node* r;              // replacement
  auto oc = u->color_;  // original color

  if (u->left_ == nil_) {
    r = u->right_;
    TransplantSubtree(u, r);
  } else if (u->right_ == nil_) {
    r = u->left_;
    TransplantSubtree(u, r);
  } else {
    Node* r2 = u->right_;  // second replacement
    while (r2->left_ != nil_) r2 = r2->left_;

    oc = r2->color_;
    r = r2->right_;

    if (r2 == u->right_)  // nil-node case
      r->parent_ = r2;
    else {
      TransplantSubtree(r2, r);
      r2->right_ = u->right_;
      r2->right_->parent_ = r2;
    }

    TransplantSubtree(u, r2);
    r2->left_ = u->left_;
    r2->left_->parent_ = r2;
    r2->color_ = u->color_;
  }

  delete u, n_--;
  if (oc == Node::Color::black) EraseFixup(r);
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::TransplantSubtree(Node* dst, Node* src) {
  if (dst->parent_ == nil_)
    root_ = src;
  else if (dst->is_left())
    dst->parent_->left_ = src;
  else
    dst->parent_->right_ = src;
  src->parent_ = dst->parent_;
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::EraseFixup(Node* node) {
  if (node == root_) return;
  Node* u = node;
  Node* p = u->parent();
  Node* s = u->sibling();  // sibling
  if (s->is_black()) {
    if (s->left_->is_red() || s->right_->is_red()) {
      Node* n = (s->left_->is_red() ? s->left_ : s->right_);  // nephew
      auto top_color = p->color_;
      n = TrinodeRestructure(n);
      n->set_color(top_color);
      u->set_black();
      n->left_->set_black();
      n->right_->set_black();
    } else {
      u->set_black();
      s->set_red();
      if (p->is_black() && p != root_) EraseFixup(p);
      p->set_black();
    }
  } else {
    Node* n = (s->is_right() ? s->right_ : s->left_);
    n = TrinodeRestructure(n);
    u->parent_ = p;  // restore nil-node parent after restructure
    s->set_black();
    p->set_red();
    EraseFixup(u);
  }
}

template <class K, class V, class C>
typename RBTree<K, V, C>::Node* RBTree<K, V, C>::TrinodeRestructure(
    Node* node) {
  Node* n = node;
  Node* p = n->parent();
  Node* g = n->grandparent();

  Node *a, *b, *c;          // inorder naming of n, p, g
  Node *t1, *t2, *t3, *t4;  // inorder naming of subtrees not rooted at n, p, g
  if (g->right_ == p && p->right_ == n)
    a = g, b = p, c = n, t2 = b->left_, t3 = c->left_;
  if (g->right_ == p && p->left_ == n)
    a = g, b = n, c = p, t2 = b->left_, t3 = b->right_;
  if (g->left_ == p && p->left_ == n)
    a = n, b = p, c = g, t2 = a->right_, t3 = b->right_;
  if (g->left_ == p && p->right_ == n)
    a = p, b = n, c = g, t2 = b->left_, t3 = b->right_;

  TransplantSubtree(g, b);
  b->left_ = a, b->right_ = c, a->parent_ = c->parent_ = b;
  a->right_ = t2, t2->parent_ = a;
  c->left_ = t3, t3->parent_ = c;
  (void)t1, (void)t4;  // t1, t4 retain their initial positions

  return b;
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::Merge(RBTree& other) {
  const RBTree ctree(other);
  for (const auto& elem : ctree) {
    auto pos = Insert(elem);
    other.erase(*pos);
  }
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::MergeIfNe(RBTree& other) {
  const RBTree ctree(other);
  for (const auto& elem : ctree) {
    auto [pos, res] = InsertIfNe(elem);
    if (res) other.erase(*pos);
  }
}

template <class Key, class Value, class Compare>
std::ostream& operator<<(std::ostream& os,
                         const RBTree<Key, Value, Compare>& t) noexcept {
  t.Print(os);
  return os;
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::Print(std::ostream& os) const noexcept {
  os << "N" << std::endl;
  if (n_) PrintRecursive(os, "", root_, true);
}

template <class T1, class T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p) {
  os << '{' << p.first << ',' << p.second << '}';
  return os;
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::PrintRecursive(std::ostream& os,
                                                 const std::string& prefix,
                                                 Node* node,
                                                 bool is_left) const noexcept {
  if (!node) return;

  os << prefix;
  os << (is_left ? "├──" : "└──");

  if (node == nil_)
    os << "N";
  else
    os << (node->is_red() ? "R" : "B") << ":" << node->data_;
  os << std::endl;

  if (node == nil_) return;
  PrintRecursive(os, prefix + (is_left ? "│   " : "    "), node->left_, true);
  PrintRecursive(os, prefix + (is_left ? "│   " : "    "), node->right_, false);
}

// template <class Key, class Value, class Compare>
// std::enable_if_t<!(std::is_void_v<Value>), Value&>
// RBTree<Key, Value, Compare>::at(const Key& key) {
//   auto pos = Find(key);
//   if (pos == end()) throw std::out_of_range("The entry does not exist.");
//   return (*pos).second;
// }

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::swap(RBTree& other) noexcept {
  using std::swap;
  swap(root_, other.root_);
  swap(nil_, other.nil_);
  swap(n_, other.n_);
}

// ITERATOR

template <class Key, class Value, class Compare>
template <bool is_const>
RBTree<Key, Value, Compare>::Iterator<is_const>::Iterator(node_pointer p)
    : ptr_(p) {}

template <class K, class V, class C>
typename RBTree<K, V, C>::iterator RBTree<K, V, C>::begin() const {
  if (n_ == 0) return end();
  Node* tmp = root_;
  while (tmp->left_->is_internal()) tmp = tmp->left_;
  return iterator(tmp);
}

template <class K, class V, class C>
typename RBTree<K, V, C>::iterator RBTree<K, V, C>::end() const {
  return iterator(nil_);
}

template <class K, class V, class C>
typename RBTree<K, V, C>::const_iterator RBTree<K, V, C>::cbegin() const {
  return begin();
}

template <class K, class V, class C>
typename RBTree<K, V, C>::const_iterator RBTree<K, V, C>::cend() const {
  return end();
}

template <class Key, class Value, class Compare>
template <bool is_const>
typename RBTree<Key, Value, Compare>::template Iterator<is_const>::reference
RBTree<Key, Value, Compare>::Iterator<is_const>::operator*() const {
  return ptr_->data_;
}

template <class K, class V, class C>
template <bool is_const>
auto& RBTree<K, V, C>::Iterator<is_const>::operator++() {
  Node* nptr = ptr_->right_;
  if (nptr->is_internal()) {
    do {
      ptr_ = nptr;
      nptr = ptr_->left_;
    } while (nptr->is_internal());
  } else {
    nptr = ptr_->parent_;
    while (ptr_ == nptr->right_) {
      ptr_ = nptr;
      nptr = ptr_->parent_;
    }
    ptr_ = nptr;
  }
  return *this;
}

template <class Key, class Value, class Compare>
template <bool is_const>
auto RBTree<Key, Value, Compare>::Iterator<is_const>::operator++(int) {
  auto result = *this;
  ++*this;
  return result;
}

template <class Key, class Value, class Compare>
template <bool is_const>
template <bool R>
bool RBTree<Key, Value, Compare>::Iterator<is_const>::operator==(
    const Iterator<R>& rhs) const noexcept {
  return ptr_ == rhs.ptr_;
}

template <class Key, class Value, class Compare>
template <bool is_const>
template <bool R>
bool RBTree<Key, Value, Compare>::Iterator<is_const>::operator!=(
    const Iterator<R>& rhs) const noexcept {
  return ptr_ != rhs.ptr_;
}

template <class Key, class Value, class Compare>
template <bool is_const>
RBTree<Key, Value, Compare>::Iterator<is_const>::operator Iterator<true>()
    const noexcept {
  return Iterator<true>{ptr_};
}

// NODE

template <class Key, class Value, class Compare>
RBTree<Key, Value, Compare>::Node::Node(const value_type& v, Color c)
    : data_(v), color_(c) {}

template <class Key, class Value, class Compare>
RBTree<Key, Value, Compare>::Node::Node(const value_type& v, Color c, Node* p)
    : data_(v), color_(c), parent_(p) {}

template <class Key, class Value, class Compare>
bool RBTree<Key, Value, Compare>::Node::is_internal() const noexcept {
  return left_ && right_;
}

template <class Key, class Value, class Compare>
bool RBTree<Key, Value, Compare>::Node::is_external() const noexcept {
  return !right_;
}

template <class Key, class Value, class Compare>
bool RBTree<Key, Value, Compare>::Node::is_left() const noexcept {
  return parent_->left_ == this;
}

template <class Key, class Value, class Compare>
inline bool RBTree<Key, Value, Compare>::Node::is_right() const noexcept {
  return parent_->right_ == this;
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::Node*
RBTree<Key, Value, Compare>::Node::sibling() const noexcept {
  return this->is_left() ? parent_->right_ : parent_->left_;
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::Node*
RBTree<Key, Value, Compare>::Node::parent() const noexcept {
  return parent_;
}

template <class Key, class Value, class Compare>
typename RBTree<Key, Value, Compare>::Node*
RBTree<Key, Value, Compare>::Node::grandparent() const noexcept {
  return parent_->parent_;
}

template <class Key, class Value, class Compare>
bool RBTree<Key, Value, Compare>::Node::is_black() const noexcept {
  return color_ == Color::black;
}

template <class Key, class Value, class Compare>
bool RBTree<Key, Value, Compare>::Node::is_red() const noexcept {
  return color_ == Color::red;
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::Node::set_black() noexcept {
  color_ = Color::black;
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::Node::set_red() noexcept {
  color_ = Color::red;
}

template <class Key, class Value, class Compare>
void RBTree<Key, Value, Compare>::Node::set_color(Color col) noexcept {
  color_ = col;
}

template <class Key, class Value, class Compare>
template <class... Args>
std::vector<std::pair<typename RBTree<Key, Value, Compare>::iterator, bool>>
RBTree<Key, Value, Compare>::InsertMany(Args&&... args) {
  using namespace std;
  vector<pair<typename RBTree<Key, Value, Compare>::iterator, bool>> vec;
  for (auto&& arg : {args...}) {
    vec.push_back({Insert(arg), true});
  }
  return vec;
}

template <class Key, class Value, class Compare>
template <class... Args>
std::vector<std::pair<typename RBTree<Key, Value, Compare>::iterator, bool>>
RBTree<Key, Value, Compare>::InsertManyIfNe(Args&&... args) {
  using namespace std;
  vector<pair<typename RBTree<Key, Value, Compare>::iterator, bool>> vec;
  for (auto&& arg : {args...}) {
    vec.push_back(InsertIfNe(arg));
  }
  return vec;
}

}  // namespace s21
#endif  // RBTREE_H_
