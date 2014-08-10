#include "etude/utility/forward.hpp"

#include <utility>
#include <gtest/gtest.h>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

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

TEST(move, constexpr) {
  constexpr int i = 0;
  constexpr int j = ETUDE_FORWARD(i);
  STATIC_ASSERT(i == j);
}
