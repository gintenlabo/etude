#include "etude/utility/move.hpp"

#include <memory>
#include <gtest/gtest.h>

TEST(move, trivial) {
  std::unique_ptr<int> p1(new int());
  ASSERT_TRUE(static_cast<bool>(p1));

  auto p2 = etude::move(p1);
  EXPECT_FALSE(static_cast<bool>(p1));
  EXPECT_TRUE(static_cast<bool>(p2));

  auto p3 = etude::move(std::unique_ptr<int>{new int()});
  EXPECT_TRUE(static_cast<bool>(p3));
}
