
#include <utility>

#include "gtest/gtest.h"
#include "s21_containers/s21_set.h"

template class s21::set<int>;

TEST(SetTest, InsertMethod) {
  s21::set<int> s = {1, 2, 3};

  auto res_success = s.insert(4);
  EXPECT_EQ(res_success.second, true);
  EXPECT_EQ(*res_success.first, 4);

  auto res_failure = s.insert(4);
  EXPECT_EQ(res_failure.second, false);
}

TEST(SetTest, EraseMethod) {
  s21::set<int> s = {4, 5, 6};

  s.erase(4);
  s.erase(5);
  s.erase(6);
  EXPECT_EQ(s.size(), 0);
}

TEST(SetTest, MergeMethod) {
  s21::set<int> s1 = {1, 3};
  s21::set<int> s2 = {-1, 3, 2, 5, 6};

  s1.merge(s2);
  EXPECT_EQ(s1.size(), 6);
  EXPECT_EQ(s2.size(), 1);
  EXPECT_TRUE(s2.contains(3));
}

TEST(SetTest, CopyAssignmentOperator) {
  s21::set<int> s1;
  const s21::set<int> s2 = {1, 2, 3};

  s1 = s2;
  EXPECT_EQ(s1.size(), s2.size());
}

TEST(SetTest, MoveAssignmentOperator) {
  s21::set<int> s1;
  s21::set<int> s2 = {1, 2, 3};
  auto s2_size = s2.size();

  s1 = std::move(s2);
  EXPECT_EQ(s1.size(), s2_size);
}

TEST(SetTest, insertManyMethod) {
  s21::set<int> s = {1, 2, 3};

  s.insert_many(4, 5, 6);
  EXPECT_EQ(s.size(), 6);
}
