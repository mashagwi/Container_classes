#include <algorithm>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "gtest/gtest.h"
#include "s21_containers/detail/s21_rbtree.h"

TEST(SearchTreeTest, OneTemplateParameterInitializerList) {
  const s21::search_tree<int> t = {1, 2, 3};
  EXPECT_EQ(t.size(), 3);
  EXPECT_EQ(t.empty(), false);
}

TEST(SearchTreeTest, TwoTemplateParametersInitializerList) {
  const s21::search_tree<int, std::string> t = {
      {1, "one"}, {2, "two"}, {3, "three"}};
  EXPECT_EQ(t.size(), 3);
  EXPECT_EQ(t.empty(), false);
}

TEST(SearchTreeTest, CopyConstructor) {
  const s21::search_tree<int> t1 = {4, 7, 12, 15, 3, 5, 14, 18, 16, 17};

  const s21::search_tree<int> t2(t1);
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

TEST(SearchTreeTest, IsEmptyInitially) {
  const s21::search_tree<int> t;
  EXPECT_EQ(t.size(), 0);
  EXPECT_EQ(t.empty(), true);
}

TEST(SearchTreeTest, ProperOrder) {
  auto il = {4, 7, 12, 15, 3, 5, 14, 18, 16, 17};  // NOLINT
  std::vector<int> vec = il;
  sort(vec.begin(), vec.end());

  s21::search_tree<int> tree = il;
  ASSERT_EQ(tree.size(), vec.size());

  int i = 0;
  for (auto iter = tree.begin(); iter != tree.end(); ++iter, ++i) {
    ASSERT_EQ(*iter, vec[i]);
  }

  tree.erase(3);
  tree.erase(12);
  tree.erase(12);
  tree.erase(17);
  tree.erase(18);
  tree.erase(15);
  tree.erase(16);

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
  const s21::search_tree<int> t = {1, 2, 3};
  EXPECT_TRUE(t.begin() == t.cbegin());
  EXPECT_TRUE(t.begin() != t.cend());
}

TEST(SearchTreeTest, IteratorIteration) {
  const s21::search_tree<int> t = {1, 2, 3};

  auto iter = t.begin();
  ASSERT_EQ(*iter, 1);
  iter++;
  ASSERT_EQ(*iter, 2);
  ++iter;
  ASSERT_EQ(*iter, 3);
  ++iter;
  ASSERT_EQ(iter, t.end());
}

TEST(SearchTreeTest, AtMethod) {
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
  EXPECT_EQ(t[1], "one");
  EXPECT_EQ(t[2], "two");
  EXPECT_EQ(t[3], "three");
  EXPECT_EQ(t[4], "");
  EXPECT_EQ(t[5], "");
  t[5] = "five";
  EXPECT_EQ(t[5], "five");
}
