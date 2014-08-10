#include "etude/utility/as.hpp"

#include <utility>
#include <gtest/gtest.h>
#include "etude/utility/forward.hpp"

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
