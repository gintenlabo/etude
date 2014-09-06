#include "etude/utility/immutable.hpp"

#include <utility>
#include <gtest/gtest.h>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

TEST(immutable, default_construct) {
  etude::immutable<int> const x = {};
  EXPECT_FALSE(static_cast<bool>(x));
  EXPECT_FALSE(x.unique());
  EXPECT_EQ(nullptr, x.get_ptr());
  EXPECT_EQ(nullptr, x.get_shared());
}

TEST(immutable, make_immutable) {
  auto const x = etude::make_immutable<int>(0);
  ASSERT_TRUE(static_cast<bool>(x));
  EXPECT_EQ(0, *x);
  EXPECT_TRUE(x.unique());
  EXPECT_EQ(x.get_ptr(), x.get_shared().get());
  EXPECT_EQ(x.get_ptr(), x.operator->());
  EXPECT_EQ(x.get_ptr(), &*x);
}

TEST(immutable, accept_shared_empty) {
  etude::immutable<int> const x(std::shared_ptr<int>{});
  EXPECT_FALSE(static_cast<bool>(x));
  EXPECT_FALSE(x.unique());
}

TEST(immutable, accept_shared_unique) {
  // unique shared_ptr
  auto p = std::make_shared<int>(0);
  auto p_ = p.get();
  ASSERT_TRUE(p.unique());

  etude::immutable<int> const x(std::move(p));
  EXPECT_FALSE(static_cast<bool>(p));  // moved
  EXPECT_TRUE(x.unique());
  EXPECT_EQ(p_, x.get_ptr());
}

TEST(immutable, accept_shared_non_unique) {
  // non-unique shared_ptr
  auto p = std::make_shared<int>(0);
  auto p_ = p;
  ASSERT_FALSE(p.unique());

  EXPECT_THROW({
    etude::immutable<int> const x(std::move(p));
  }, std::invalid_argument);
}

TEST(immutable, modifiers) {
  auto x = etude::make_immutable<int>(1);
  auto y = etude::make_immutable<int>(2);
  auto px = x.get_ptr();
  auto py = y.get_ptr();
  ASSERT_NE(px, py);

  // swap itself
  swap(x, x);
  ASSERT_EQ(px, x.get_ptr());

  // swap two objects
  x.swap(y);
  ASSERT_EQ(py, x.get_ptr());
  ASSERT_EQ(px, y.get_ptr());
  swap(x, y);
  ASSERT_EQ(px, x.get_ptr());
  ASSERT_EQ(py, y.get_ptr());

  // move
  auto z = std::move(x);
  ASSERT_EQ(nullptr, x.get_ptr());
  ASSERT_EQ(px, z.get_ptr());

  // move assign
  x = std::move(z);
  ASSERT_EQ(nullptr, z.get_ptr());
  ASSERT_EQ(px, x.get_ptr());
}

TEST(immutable, modify_with) {
  auto x = etude::make_immutable<int>(0);
  auto y = x;

  EXPECT_EQ(0, *x);
  EXPECT_EQ(0, *y);

  auto inc = [] (int& i) { return ++i; };

  auto r = x.modify_with(inc);
  EXPECT_EQ(1, r);
  EXPECT_EQ(1, *x);
  EXPECT_EQ(0, *y);

  ASSERT_TRUE(x.unique());
  auto px = x.get_ptr();
  r = x.modify_with(inc);
  EXPECT_EQ(px, x.get_ptr());  // object is unchanged if it's unique
  EXPECT_EQ(2, r);
  EXPECT_EQ(2, *x);
  EXPECT_EQ(0, *y);
}
