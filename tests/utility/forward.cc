#include "etude/utility/forward.hpp"

#include <memory>
#include <utility>
#include <gtest/gtest.h>

template<class To, class From,
  class = decltype(etude::forward<To>(std::declval<From>()))
>
constexpr bool is_forwardable_to_(From&&, int) noexcept {
  return true;
}

template<class To, class From>
constexpr bool is_forwardable_to_(From&&, ...) noexcept {
  return false;
}

template<class To, class From>
constexpr bool is_forwardable_to(From && x) noexcept {
  return is_forwardable_to_<To>(etude::forward<From>(x), 0);
}

TEST(move, trivial) {
  std::unique_ptr<int> p1(new int());
  ASSERT_TRUE(static_cast<bool>(p1));

  auto p2 = etude::move(p1);
  EXPECT_FALSE(static_cast<bool>(p1));
  EXPECT_TRUE(static_cast<bool>(p2));

  auto p3 = etude::move(std::unique_ptr<int>{new int()});
  EXPECT_TRUE(static_cast<bool>(p3));
}

TEST(forward, lvalue) {
  int x;

  EXPECT_TRUE(is_forwardable_to<int>(x));
  EXPECT_TRUE(is_forwardable_to<int&>(x));
  EXPECT_TRUE(is_forwardable_to<int const&>(x));
  EXPECT_TRUE(is_forwardable_to<int&&>(x));
  EXPECT_TRUE(is_forwardable_to<int const&&>(x));
}

TEST(forward, rvalue) {
  EXPECT_FALSE(is_forwardable_to<int>(1));
  EXPECT_FALSE(is_forwardable_to<int&>(1));
  EXPECT_FALSE(is_forwardable_to<int const&>(1));
  EXPECT_FALSE(is_forwardable_to<int&&>(1));
  EXPECT_FALSE(is_forwardable_to<int const&&>(1));
}
