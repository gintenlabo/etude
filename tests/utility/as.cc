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

TEST(as, noexcept) {
  int i = 0;
  EXPECT_TRUE(noexcept(etude::as<int>(i)));
  EXPECT_TRUE(noexcept(etude::as<double>(i)));
  EXPECT_TRUE(noexcept(etude::as<double&&>(i)));

  struct may_throw_in_move {
    may_throw_in_move() noexcept = default;
    may_throw_in_move(may_throw_in_move const&) /*noexcept*/ {
    }
  };
  EXPECT_FALSE(noexcept(etude::as<may_throw_in_move>(may_throw_in_move{})));
  EXPECT_TRUE(noexcept(etude::as<may_throw_in_move&&>(may_throw_in_move{})));
}

TEST(as, references) {
  struct base {
    base() = default;
    base(const base&) = delete;
    void operator=(const base&) = delete;
  };
  struct derived : base {
  };

  base b = {};
  derived d = {};

  EXPECT_FALSE(fails_to_call_as<base&>(b));
  EXPECT_FALSE(fails_to_call_as<base const&>(b));
  EXPECT_TRUE(fails_to_call_as<base&&>(b));

  EXPECT_TRUE(fails_to_call_as<base&>(std::move(b)));
  EXPECT_FALSE(fails_to_call_as<base const&>(std::move(b)));
  EXPECT_FALSE(fails_to_call_as<base&&>(std::move(b)));

  EXPECT_TRUE(fails_to_call_as<derived&>(b));
  EXPECT_FALSE(fails_to_call_as<base&>(d));
}

TEST(as, references_to_temporary) {
  int i = 0;

  EXPECT_FALSE(fails_to_call_as<int&>(i));
  EXPECT_FALSE(fails_to_call_as<int const&>(i));
  EXPECT_TRUE(fails_to_call_as<int&&>(i));

  EXPECT_TRUE(fails_to_call_as<double&>(i));
  EXPECT_FALSE(fails_to_call_as<double const&>(i));
  EXPECT_FALSE(fails_to_call_as<double&&>(i));

  struct dangling_detector {
    int value;

    /*explicit*/ dangling_detector(int x)
        : value(x) {
    }

    ~dangling_detector() {
      value = 0;
    }
  };
  EXPECT_EQ(1, etude::as<dangling_detector const&>(1).value);
}
