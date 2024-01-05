#include "gtest/gtest.h"
#include "s21_containers/s21_list.h"

// explicit template class instantiation for the needs of code coverage
template class s21::list<int>;

TEST(ListCtorDefault, Size) {
  const int sz = 0;
  s21::list<int> l;
  EXPECT_TRUE(l.empty());
  EXPECT_EQ(sz, l.size());
}

//TEST(VectorCtorParametrized, Size) {
//  const int sz = 5;
//  s21::vector<int> v(sz);
//  EXPECT_EQ(sz, v.size());
//}
