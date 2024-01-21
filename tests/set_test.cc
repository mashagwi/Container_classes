
#include <algorithm>
#include <iostream>
#include <limits>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "gtest/gtest.h"
#include "s21_containers/s21_set.h"

// explicit template class instantiation for correct code coverage analysis
template class s21::set<int>;

// NOLINTBEGIN
class SetTest : public testing::Test {
 protected:
  void SetUp() override {
    auto il_ = {4, 7, 12, 15, 3, 5, 14, 18, 16, 17, -1};  // NOLINT
    iv_ = il_;
    sort(iv_.begin(), iv_.end());
    is_ = il_;
  }

  std::vector<int> iv_;
  s21::set<int> is_;
  int i_ = 0;
};
// NOLINTEND

TEST_F(SetTest, MutableIteration) {
  for (auto iter = is_.begin(); iter != is_.end(); ++iter, ++i_) {
    ASSERT_EQ(*iter, iv_[i_]);
  }
  auto iter = is_.begin();  // NOLINT
  EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(*iter)>>);
}
TEST_F(SetTest, ReverseMutableIteration) {
  std::reverse(iv_.begin(), iv_.end());
  for (auto iter = is_.rbegin(); iter != is_.rend(); ++iter, ++i_) {
    ASSERT_EQ(*iter, iv_[i_]);
  }
  auto iter = is_.rbegin();  // NOLINT
  EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(*iter)>>);
}

TEST_F(SetTest, NonMutableIteration) {
  for (auto iter = is_.cbegin(); iter != is_.cend(); ++iter, ++i_) {
    ASSERT_EQ(*iter, iv_[i_]);
  }
  auto iter = is_.cbegin();  // NOLINT
  EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(*iter)>>);
}
TEST_F(SetTest, ReverseNonMutableIteration) {
  std::reverse(iv_.begin(), iv_.end());
  for (auto iter = is_.crbegin(); iter != is_.crend(); ++iter, ++i_) {
    ASSERT_EQ(*iter, iv_[i_]);
  }
  auto iter = is_.crbegin();  // NOLINT
  EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(*iter)>>);
}

TEST_F(SetTest, NonMutableIterationConst) {
  const s21::set<int> cis = is_;
  for (auto iter = cis.begin(); iter != cis.end(); ++iter, ++i_) {
    ASSERT_EQ(*iter, iv_[i_]);
  }
  auto iter = cis.begin();  // NOLINT
  EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(*iter)>>);
}
TEST_F(SetTest, ReverseNonMutableIterationConst) {
  std::reverse(iv_.begin(), iv_.end());
  const s21::set<int> cis = is_;
  for (auto iter = cis.rbegin(); iter != cis.rend(); ++iter, ++i_) {
    ASSERT_EQ(*iter, iv_[i_]);
  }
  auto iter = cis.rbegin();  // NOLINT
  EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(*iter)>>);
}

TEST_F(SetTest, FindMethod) {
  const s21::set<int> cis = {1, 2, 3};
  auto pos = cis.find(1);
  EXPECT_EQ(*pos, 1);
}

TEST_F(SetTest, UniquenessProperty) {
  s21::set<int> is = {1, 1, 2, 3, 4};
  ASSERT_EQ(is.size(), 4);
  is.insert(3);
  ASSERT_EQ(is.size(), 4);
  is.erase(1);
  ASSERT_EQ(is.size(), 3);

  const s21::set<int> is2 = {1, 1, 1};
  ASSERT_EQ(is2.size(), 1);
}

TEST_F(SetTest, InsertMethod) {  // NOLINT
  const bool is_exist = is_.contains(4);

  auto [pos, res] = is_.insert(4);
  if (is_exist)
    EXPECT_EQ(res, false);
  else
    EXPECT_EQ(res, true);
  EXPECT_EQ(*pos, 4);

  std::tie(pos, res) = is_.insert(4);
  EXPECT_EQ(res, false);
  EXPECT_EQ(*pos, 4);

  const int imax = std::numeric_limits<int>::max();
  std::tie(pos, res) = is_.insert(imax);
  EXPECT_EQ(res, true);
  EXPECT_EQ(*pos, imax);
}

TEST_F(SetTest, EraseMethod) {
  is_.insert(4);
  const auto bsz = is_.size();
  is_.erase(4);
  EXPECT_EQ(is_.size(), bsz - 1);

  is_.insert(4);
  EXPECT_EQ(is_.size(), bsz);
  auto pos = is_.find(4);
  is_.erase(pos);
  EXPECT_EQ(is_.size(), bsz - 1);
}

TEST_F(SetTest, InsertManyMethod) {
  s21::set<int> is = {1, 2, 3};
  is.insert_many(1, 2, 3, 4);
  EXPECT_EQ(is.size(), 4);
}

TEST_F(SetTest, MergeMethod) {
  s21::set<int> s1 = {1, 3};
  s21::set<int> s2 = {-1, 3, 2, 5, 6};  // NOLINT

  s1.merge(s2);
  EXPECT_EQ(s1.size(), 6);
  EXPECT_EQ(s2.size(), 1);
  EXPECT_TRUE(s2.contains(3));
}

TEST_F(SetTest, CopyAssignmentOperator) {
  s21::set<int> s;
  s = is_;
  EXPECT_EQ(s.size(), is_.size());
}

TEST_F(SetTest, SelfCopyAssignmentOperator) {
  s21::set<int> s(is_);
  s = s;
  ASSERT_EQ(s.size(), is_.size());
  for (const auto &elem : s) {
    ASSERT_EQ(elem, iv_[i_]);
    i_++;
  }
}

TEST_F(SetTest, MoveAssignmentOperator) {
  auto s2_psize = is_.size();
  s21::set<int> s;
  s = std::move(is_);
  EXPECT_EQ(s.size(), s2_psize);
  EXPECT_TRUE(is_.empty());
}

TEST_F(SetTest, SelfMoveAssignmentOperator) {
  s21::set<int> s(is_);
  s21::set<int> *p = &s;
  s = std::move(*p);
  ASSERT_EQ(s.size(), is_.size());
  for (const auto &elem : s) {
    ASSERT_EQ(elem, iv_[i_]);
    i_++;
  }
}

TEST_F(SetTest, MoveConstructor) {
  auto s2_psize = is_.size();
  const s21::set<int> s(std::move(is_));
  EXPECT_EQ(s.size(), s2_psize);
  EXPECT_TRUE(is_.empty());
}

TEST_F(SetTest, SwapMethod) {
  s21::set<int> s;
  EXPECT_TRUE(s.empty());
  EXPECT_FALSE(is_.empty());
  s.swap(is_);
  EXPECT_FALSE(s.empty());
  EXPECT_TRUE(is_.empty());
}

TEST_F(SetTest, EmptyMethod) {
  EXPECT_FALSE(is_.empty());
  is_.clear();
  EXPECT_TRUE(is_.empty());
  EXPECT_EQ(is_.size(), 0);
}

TEST_F(SetTest, ClearMethod) {
  EXPECT_FALSE(is_.empty());
  is_.clear();
  EXPECT_TRUE(is_.empty());
}

TEST_F(SetTest, MaxSizeMethod) { std::clog << is_.max_size() << '\n'; }

TEST_F(SetTest, EmptyInitializerListConstructor) {
  const s21::set<int> s({});
  EXPECT_TRUE(s.empty());
}
