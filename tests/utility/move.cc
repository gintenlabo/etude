#include "etude/utility/move.hpp"

#include <memory>
#include <gtest/gtest.h>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

TEST(move, trivial) {
  std::unique_ptr<int> p1(new int());
  ASSERT_TRUE(static_cast<bool>(p1));

  auto p2 = etude::move(p1);
  EXPECT_FALSE(static_cast<bool>(p1));
  EXPECT_TRUE(static_cast<bool>(p2));

  auto p3 = etude::move(std::unique_ptr<int>{new int()});
  EXPECT_TRUE(static_cast<bool>(p3));
}

TEST(move, constexpr) {
  constexpr int i = 0;
  constexpr int j = etude::move(i);
  STATIC_ASSERT(i == j);
}
