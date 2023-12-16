#include "gtest/gtest.h"
#include "s21_containers/s21_vector.h"

// explicit template class instantiation for the needs of code coverage
template class s21::vector<int>;

TEST(VectorCtorDefault, Size) {
  const int sz = 0;
  s21::vector<int> v;
  // EXPECT_TRUE(v.empty());
  EXPECT_EQ(sz, v.size());
}

TEST(VectorCtorParametrized, Size) {
  const int sz = 5;
  s21::vector<int> v(sz);
  EXPECT_EQ(sz, v.size());
}
