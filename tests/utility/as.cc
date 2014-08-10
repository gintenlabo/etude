#include "etude/utility/as.hpp"

#include <utility>
#include <gtest/gtest.h>
#include "etude/utility/forward.hpp"

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template<class To, class From,
  class = decltype(etude::as<To>(std::declval<From>()))
>
constexpr bool fails_to_call_as_(From&&, int) noexcept {
  return false;
}

template<class To, class From>
constexpr bool fails_to_call_as_(From&&, ...) noexcept {
  return true;
}

template<class To, class From>
constexpr bool fails_to_call_as(From && x) noexcept {
  return fails_to_call_as_<To>(etude::forward<From>(x), 0);
}

TEST(as, trivial) {
  EXPECT_FALSE(fails_to_call_as<int>(int{}));
  EXPECT_TRUE(fails_to_call_as<void*>(int{}));

  EXPECT_EQ(nullptr, etude::as<void*>(0));
}

TEST(as, constexpr) {
  constexpr int i = etude::as<int>(0);
  constexpr double d = etude::as<double>(i);
  STATIC_ASSERT(d == i);
}

TEST(as, vector_initialization) {
  auto v = etude::as<std::vector<int>>({1, 2, 3});
  ASSERT_EQ(3u, v.size());
  EXPECT_EQ(1, v[0]);
  EXPECT_EQ(2, v[1]);
  EXPECT_EQ(3, v[2]);
}

TEST(as, immovable_class) {
  struct tested_t {
    tested_t() = default;
    tested_t(int) {}

    tested_t(const tested_t&) = delete;
    void operator=(const tested_t&) = delete;
  };

  EXPECT_TRUE(fails_to_call_as<tested_t>(int{}));
  EXPECT_TRUE(fails_to_call_as<tested_t>(tested_t{}));

  EXPECT_FALSE(fails_to_call_as<tested_t&&>(int{}));
  EXPECT_FALSE(fails_to_call_as<tested_t const&>(int{}));
  EXPECT_TRUE(fails_to_call_as<tested_t&>(int{}));
}
