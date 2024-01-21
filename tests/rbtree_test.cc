#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "s21_containers/detail/s21_rbtree.h"

// explicit template class instantiation for correct code coverage analysis
template class s21::search_tree<int>;
template class s21::search_tree<int, std::string>;

TEST(SearchTreeTest, OneTemplateParameterInitializerList) {
  const s21::search_tree<int> t = {1, 2, 3};
  EXPECT_EQ(t.size(), 3);
}

TEST(SearchTreeTest, TwoTemplateParametersInitializerList) {
  const s21::search_tree<int, std::string> t = {
      {1, "one"}, {2, "two"}, {3, "three"}};
  EXPECT_EQ(t.size(), 3);
}

TEST(SearchTreeTest, CopyConstructor) {
  const s21::search_tree<int> t1 = {4, 7, 12, 15, 3, 5, 14, 18, 16, 17};

  const s21::search_tree<int> t2(t1);  // NOLINT
  ASSERT_EQ(t2.size(), t1.size());

  auto iter1 = t1.begin();
  auto iter2 = t2.begin();
  for (; iter1 != t1.end(); ++iter1, ++iter2) {
    ASSERT_EQ(*iter2, *iter1);
  }
}

TEST(SearchTreeTest, CopyAssignment) {
  const s21::search_tree<int> t1 = {4, 7, 12, 15, 3, 5, 14, 18, 16, 17};
  s21::search_tree<int> t2;

  t2 = t1;
  ASSERT_EQ(t2.size(), t1.size());

  auto iter1 = t1.begin();
  auto iter2 = t2.begin();
  for (; iter1 != t1.end(); ++iter1, ++iter2) {
    ASSERT_EQ(*iter2, *iter1);
  }
}

TEST(SearchTreeTest, MoveAssignment) {
  s21::search_tree<int> t1 = {4, 7, 12, 15, 3, 5, 14, 18, 16, 17};  // NOLINT
  s21::search_tree<int> ref(t1);
  s21::search_tree<int> t2;

  t2 = std::move(t1);
  ASSERT_EQ(t2.size(), ref.size());

  auto iter1 = ref.begin();
  auto iter2 = t2.begin();
  for (; iter1 != ref.end(); ++iter1, ++iter2) {
    ASSERT_EQ(*iter2, *iter1);
  }
}

TEST(SearchTreeTest, IsEmptyInitially) {
  const s21::search_tree<int> t;
  EXPECT_EQ(t.size(), 0);
  EXPECT_EQ(t.empty(), true);
}

TEST(SearchTreeTest, ProperOrderDemonstration) {
  auto il = {4, 7, 12, 15, 3, 5, 14, 18, 16, 17};  // NOLINT
  std::vector<int> vec = il;
  sort(vec.begin(), vec.end());

  s21::search_tree<int> tree = il;
  ASSERT_EQ(tree.size(), vec.size());

  int i = 0;
  for (auto iter = tree.begin(); iter != tree.end(); ++iter, ++i) {
    ASSERT_EQ(*iter, vec[i]);
  }

  // NOLINTBEGIN
  tree.erase(3);
  tree.erase(12);
  tree.erase(12);
  tree.erase(17);
  tree.erase(18);
  tree.erase(15);
  tree.erase(16);
  // NOLINTEND

  tree.clear();
  EXPECT_EQ(tree.size(), 0);
  EXPECT_EQ(tree.empty(), true);
}

TEST(SearchTreeTest, IteratorUnderlyingConstness) {
  using citer_ptr = s21::search_tree<int>::const_iterator::pointer;
  using citer_type = std::remove_pointer_t<citer_ptr>;
  EXPECT_TRUE(std::is_const_v<citer_type>);

  using citer_ref = s21::search_tree<int>::const_iterator::reference;
  using citer_type2 = std::remove_reference_t<citer_ref>;
  EXPECT_TRUE(std::is_const_v<citer_type2>);
}

TEST(SearchTreeTest, IteratorComparisons) {
  s21::search_tree<int> t = {1, 2, 3};  // NOLINT
  EXPECT_TRUE(t.begin() == t.cbegin());
  EXPECT_TRUE(t.begin() != t.cend());
}

TEST(SearchTreeTest, IteratorIteration) {  // NOLINT
  s21::search_tree<int> t = {1, 2, 3};

  auto iter = t.begin();
  ASSERT_EQ(*iter, 1);
  iter++;
  ASSERT_EQ(*iter, 2);
  iter--;
  ASSERT_EQ(*iter, 1);
  ++iter;
  ++iter;
  ASSERT_EQ(*iter, 3);
  ++iter;
  ASSERT_EQ(iter, t.end());

  auto riter = t.rbegin();
  ASSERT_EQ(*riter, 3);
  riter++;
  ASSERT_EQ(*riter, 2);
  --riter;
  ++riter;
  ASSERT_EQ(*riter, 2);
  ++riter;
  ASSERT_EQ(*riter, 1);
  ++riter;
  ASSERT_EQ(riter, t.rend());
}

TEST(SearchTreeTest, AtMethod) {  // NOLINT
  s21::search_tree<int, std::string> t = {{1, "one"}, {2, "two"}, {3, "three"}};
  EXPECT_EQ(t.at(1), "one");
  t.at(1) = "not one";
  EXPECT_EQ(t.at(1), "not one");
  EXPECT_EQ(t.at(2), "two");
  EXPECT_EQ(t.at(3), "three");
  ASSERT_THROW(t.at(4), std::out_of_range);  // NOLINT
}

TEST(SearchTreeTest, BracketsOperator) {
  s21::search_tree<int, std::string> t = {{1, "one"}, {2, "two"}, {3, "three"}};
  // NOLINTBEGIN
  EXPECT_EQ(t[1], "one");
  EXPECT_EQ(t[2], "two");
  EXPECT_EQ(t[3], "three");
  EXPECT_EQ(t[4], "");
  EXPECT_EQ(t[5], "");
  t[5] = "five";
  EXPECT_EQ(t[5], "five");
  // NOLINTEND
}

TEST(SearchTreeTest, InsertOrAssignMethod) {
  s21::search_tree<int, std::string> t = {{1, "one"}, {2, "two"}, {3, "three"}};

  auto [pos, res] = t.insert_or_assign(4, std::string("four"));
  EXPECT_EQ(res, true);
  EXPECT_EQ((*pos).second, "four");

  std::tie(pos, res) = t.insert_or_assign(4, std::string("five"));
  EXPECT_EQ(res, false);
  EXPECT_EQ(t[4], "five");
}

TEST(SearchTreeTest, InsertIfNe2params) {
  s21::search_tree<int, std::string> t = {{1, "one"}, {2, "two"}, {3, "three"}};

  auto [pos, res] = t.insert_if_ne(4, std::string("four"));
  EXPECT_EQ(res, true);
  EXPECT_EQ((*pos).second, "four");

  std::tie(pos, res) = t.insert_if_ne(4, std::string("five"));
  EXPECT_EQ(res, false);
  EXPECT_EQ(t[4], "four");
}

TEST(SearchTreeTest, BoundMethods) {  // NOLINT
  s21::search_tree<int, std::string> t = {
      {1, "one"}, {2, "two"}, {2, "two"}, {2, "two"}, {3, "three"}};

  ASSERT_EQ(t.count(2), 3);

  auto posl = t.lower_bound(2);
  using value_type = s21::search_tree<int, std::string>::value_type;
  ASSERT_EQ(*posl, value_type(2, "two"));
  ASSERT_EQ((*posl).first, 2);
  ASSERT_EQ((*posl).second, "two");

  auto posu = t.upper_bound(2);
  ASSERT_EQ(*posu, value_type(3, "three"));

  auto [eql, equ] = t.equal_range(2);
  EXPECT_EQ(posl, eql);
  EXPECT_EQ(posu, equ);

  posu--;
  ASSERT_EQ(*posu, value_type(2, "two"));
}

TEST(SearchTreeTest, PrintMethod) {
  const s21::search_tree<int, std::string> t = {
      {1, "one"}, {2, "two"}, {3, "three"}};
  std::clog << t;
  std::clog << t.max_size() << '\n';
  s21::search_tree<int> t2 = {5, 1, 6, 1234, 65, 1, 54, 0};  // NOLINT
  std::clog << t2;
  std::clog << t2.max_size() << '\n';
}

TEST(SearchTreeTest, MergeMethod) {
  s21::search_tree<int> t = {1, 2, 3};   // NOLINT
  s21::search_tree<int> t2 = {3, 4, 5};  // NOLINT
  t.merge(t2);
  t.merge(t2);
  EXPECT_EQ(t.size(), 6);
  EXPECT_EQ(t2.size(), 0);
  s21::search_tree<int> t3;
  t.merge(t3);
  EXPECT_EQ(t.size(), 6);
  EXPECT_EQ(t3.size(), 0);
}

int rand_i(unsigned i) {
  // NOLINTBEGIN
  i = (i << 13) ^ i;
  return ((i * (i * i * 15731 + 789221) + 1376312589) & 0x7fffffff);
  // NOLINTEND
}
TEST(SearchTreeTest, LargeOrderingTest) {
  std::vector<int> v;
  s21::search_tree<int> s;
  for (int i = 0; i < 1000; i++) {  // NOLINT
    const int rand = rand_i(i);
    v.push_back(rand);
    s.insert(rand);
  }
  sort(v.begin(), v.end());

  int i = 0;
  for (const auto& elem : s) {
    std::cerr << "FUCK" << '\n';
    std::cerr << i;
    std::cerr << "FUCK" << '\n';
    ASSERT_EQ(elem, v[i]);
    ++i;
  }
}
