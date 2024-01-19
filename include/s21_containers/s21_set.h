#ifndef S21_SET_H_
#define S21_SET_H_

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <utility>

#include "detail/s21_rbtree.h"

namespace s21 {

template <class Key, class Compare = std::less<Key>>
class set : public RBTree<Key, void, Compare> {
  using tree_type = RBTree<Key, void, Compare>;

 public:
  using key_type = Key;
  using value_type = Key;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = typename tree_type::iterator;
  using const_iterator = typename tree_type::const_iterator;
  using size_type = size_t;

  set() = default;
  set(std::initializer_list<value_type> const &items);
  set(const set &other);
  set &operator=(const set &other);
  set(set &&other) noexcept;
  set &operator=(set &&other) noexcept;
  ~set() = default;

  std::pair<iterator, bool> insert(const value_type &value);
  template <class... Args>
  std::vector<std::pair<iterator, bool>> insert_many(Args &&...args);
  void merge(set &other);
  iterator find(const Key &key);
};

template <class Key, class Compare>
set<Key, Compare>::set(const set &other) : RBTree<Key, void, Compare>(other) {}

template <class Key, class Compare>
typename set<Key, Compare>::set &set<Key, Compare>::operator=(
    const set &other) {
  tree_type::operator=(other);
  return *this;
}

template <class Key, class Compare>
typename set<Key, Compare>::set &set<Key, Compare>::operator=(
    set &&other) noexcept {
  tree_type::operator=(std::move(other));
  return *this;
}

template <class Key, class Compare>
set<Key, Compare>::set(std::initializer_list<value_type> const &items)
    : RBTree<Key, void, Compare>(items) {}

template <class Key, class Compare>
set<Key, Compare>::set(set &&other) noexcept
    : RBTree<Key, void, Compare>(std::move(other)) {}

template <class Key, class Compare>
std::pair<typename set<Key, Compare>::iterator, bool> set<Key, Compare>::insert(
    const value_type &value) {
  return tree_type::InsertIfNe(value);
}

template <class Key, class Compare>
typename set<Key, Compare>::iterator set<Key, Compare>::find(const Key &key) {
  return tree_type::Find(key);
}

template <class Key, class Compare>
void set<Key, Compare>::merge(set &other) {
  tree_type::MergeIfNe(other);
}

template <class Key, class Compare>
template <class... Args>
std::vector<std::pair<typename set<Key, Compare>::iterator, bool>>
set<Key, Compare>::insert_many(Args &&...args) {
  return tree_type::InsertManyIfNe(args...);
}

}  // namespace s21
#endif  // S21_SET_H_
