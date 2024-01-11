#include <string>

#include "gtest/gtest.h"
#include "s21_containers/s21_list.h"

// explicit template class instantiation for the needs of code coverage
template class s21::list<int>;

TEST(ListCtor, Default) {
  const int sz = 0;
  s21::list<int> l;
  EXPECT_TRUE(l.empty());
  EXPECT_EQ(sz, l.size());
}

TEST(ListCtor, Count1) {
  const int sz = 1;
  s21::list<std::string> l(sz);
  EXPECT_FALSE(l.empty());
  EXPECT_EQ(sz, l.size());
}

TEST(ListCtor, Count2) {
  const int sz = 10;
  s21::list<std::string> l(sz);
  EXPECT_FALSE(l.empty());
  EXPECT_EQ(sz, l.size());
}

TEST(ListCtor, InitializerList1) {
  const int sz = 0;
  s21::list<int> l{};
  EXPECT_TRUE(l.empty());
  EXPECT_EQ(sz, l.size());
}

TEST(ListCtor, InitializerList2) {
  const int sz = 1;
  const int f = 10;
  s21::list<int> l{10};
  EXPECT_FALSE(l.empty());
  EXPECT_EQ(sz, l.size());
  EXPECT_EQ(l.front(), f);
}

TEST(ListCtor, InitializerList3) {
  const int sz = 5;
  const int f = 1, b = 5;
  s21::list<int> l{1, 2, 3, 4, 5};
  EXPECT_FALSE(l.empty());
  EXPECT_EQ(sz, l.size());
  EXPECT_EQ(l.front(), f);
  EXPECT_EQ(l.back(), b);
}

TEST(ListCtor, Copy1) {
  const int sz = 0;
  s21::list<int> l1;
  s21::list<int> l2(l1);
  EXPECT_TRUE(l2.empty());
  EXPECT_EQ(sz, l1.size());
}

TEST(ListCtor, Copy2) {
  const int sz = 1;
  const int f = 10;
  s21::list<int> l1{f};
  s21::list<int> l2(l1);
  EXPECT_FALSE(l2.empty());
  EXPECT_EQ(sz, l2.size());
  EXPECT_EQ(l2.front(), f);
}

TEST(ListCtor, Copy3) {
  const int sz = 3;
  const int f = 1, b = 3;
  s21::list<int> l1{1, 2, 3};
  s21::list<int> l2(l1);
  EXPECT_FALSE(l2.empty());
  EXPECT_EQ(sz, l2.size());
  EXPECT_EQ(l2.front(), f);
  EXPECT_EQ(l2.back(), b);
}

TEST(ListCtor, Move1) {
  const int sz = 0;
  s21::list<int> l1;
  s21::list<int> l2(std::move(l1));
  EXPECT_TRUE(l2.empty());
  EXPECT_EQ(sz, l1.size());
}

TEST(ListCtor, Move2) {
  const int sz = 1;
  const int f = 10;
  s21::list<int> l1{f};
  s21::list<int> l2(std::move(l1));
  EXPECT_TRUE(l1.empty());
  EXPECT_FALSE(l2.empty());
  EXPECT_EQ(0, l1.size());
  EXPECT_EQ(sz, l2.size());
  EXPECT_EQ(l2.front(), f);
}

TEST(ListCtor, Move3) {
  const int sz = 3;
  const int f = 1, b = 3;
  s21::list<int> l1{1, 2, 3};
  s21::list<int> l2(std::move(l1));
  EXPECT_TRUE(l1.empty());
  EXPECT_FALSE(l2.empty());
  EXPECT_EQ(sz, l2.size());
  EXPECT_EQ(0, l1.size());
  EXPECT_EQ(l2.front(), f);
  EXPECT_EQ(l2.back(), b);
}

TEST(ListAssignment, Assign1) {
  const int sz = 0;
  s21::list<int> l1;
  s21::list<int> l2;
  l2 = std::move(l1);
  EXPECT_TRUE(l1.empty());
  EXPECT_TRUE(l2.empty());
  EXPECT_EQ(sz, l1.size());
  EXPECT_EQ(sz, l2.size());
}

TEST(ListAssignment, Assign2) {
  const int sz = 1;
  const int f = 10;
  s21::list<int> l1{f};
  s21::list<int> l2;
  l2 = (std::move(l1));
  EXPECT_TRUE(l1.empty());
  EXPECT_FALSE(l2.empty());
  EXPECT_EQ(0, l1.size());
  EXPECT_EQ(sz, l2.size());
  EXPECT_EQ(l2.front(), f);
}

TEST(ListAsssignment, Assign3) {
  const int sz = 3;
  const int f = 1, b = 3;
  s21::list<int> l1{1, 2, 3};
  s21::list<int> l2;
  l2 = (std::move(l1));
  EXPECT_TRUE(l1.empty());
  EXPECT_FALSE(l2.empty());
  EXPECT_EQ(sz, l2.size());
  EXPECT_EQ(0, l1.size());
  EXPECT_EQ(l2.front(), f);
  EXPECT_EQ(l2.back(), b);
}

TEST(ListMembers, Front1) {
  std::string s = "Hello";
  s21::list<std::string> l{s};
  EXPECT_EQ(l.front(), s);
}

TEST(ListMembers, Front2) {
  std::string s1 = "Hi";
  std::string s2 = ", ";
  std::string s3 = "World!";
  s21::list<std::string> l{s1, s2, s3};
  EXPECT_EQ(l.front(), s1);
}

TEST(ListMembers, ConstFront1) {
  std::string s = "Hello";
  const s21::list<std::string> l{s};
  EXPECT_EQ(l.front(), s);
}

TEST(ListMembers, ConstFront2) {
  std::string s1 = "Hi";
  std::string s2 = ", ";
  std::string s3 = "World!";
  const s21::list<std::string> l{s1, s2, s3};
  EXPECT_EQ(l.front(), s1);
}

TEST(ListMembers, Back1) {
  std::string s = "Hello";
  s21::list<std::string> l{s};
  EXPECT_EQ(l.back(), s);
}

TEST(ListMembers, Back2) {
  std::string s1 = "Hi";
  std::string s2 = ", ";
  std::string s3 = "World!";
  s21::list<std::string> l{s1, s2, s3};
  EXPECT_EQ(l.back(), s3);
}

TEST(ListMembers, ConstBack1) {
  std::string s = "Hello";
  const s21::list<std::string> l{s};
  EXPECT_EQ(l.back(), s);
}

TEST(ListMembers, ConstBack2) {
  std::string s1 = "Hi";
  std::string s2 = ", ";
  std::string s3 = "World!";
  const s21::list<std::string> l{s1, s2, s3};
  EXPECT_EQ(l.back(), s3);
}

TEST(ListMembers, Begin1) {
  std::string s = "Hello";
  s21::list<std::string> l;
  auto it = l.begin();
  l.push_back(s);
  EXPECT_EQ(*--it, s);
}

TEST(ListMembers, Begin2) {
  std::string s = "Hello";
  s21::list<std::string> l{s};
  EXPECT_EQ(*l.begin(), s);
}

TEST(ListMembers, Begin3) {
  std::string s1 = "Hi";
  std::string s2 = ", ";
  std::string s3 = "World!";
  s21::list<std::string> l{s1, s2, s3};
  EXPECT_EQ(*l.begin(), s1);
  EXPECT_EQ(*++l.begin(), s2);
}
