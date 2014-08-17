#include "etude/utility/enable_if.hpp"

#include <type_traits>
#include <utility>
#include <gtest/gtest.h>

#define STATIC_ASSERT(...) static_assert(__VA_ARGS__, #__VA_ARGS__)

template<class T,
    ETUDE_ENABLE_IF(std::is_integral<T>::value)>
constexpr bool is_integral(T const&) {
  return true;
}
template<class T,
    ETUDE_ENABLE_IF(!std::is_integral<T>::value)>
constexpr bool is_integral(T const&) {
  return false;
}

TEST(enable_if, trivial) {
  EXPECT_TRUE(::is_integral(0));
  EXPECT_FALSE(::is_integral(0.0));
}

template<class T,
    ETUDE_ENABLE_IF(sizeof(T) > sizeof(void*))>
constexpr bool is_bigger_than_pointer(T const&) {
  return true;
}
template<class T,
    ETUDE_ENABLE_IF(sizeof(T) <= sizeof(void*))>
constexpr bool is_bigger_than_pointer(T const&) {
  return false;
}

TEST(enable_if, using_greater_than) {
  EXPECT_TRUE(is_bigger_than_pointer(std::make_pair((void*)0, 1)));
  EXPECT_FALSE(is_bigger_than_pointer((char)0));
}

template<class T, class U,
    ETUDE_ENABLE_IF(std::is_same<T, U>::value)>
constexpr bool is_same(T const&, U const&) {
  return true;
}
template<class T, class U,
    ETUDE_ENABLE_IF(!std::is_same<T, U>::value)>
constexpr bool is_same(T const&, U const&) {
  return false;
}

TEST(enable_if, using_comma) {
  EXPECT_TRUE(::is_same(0, 0));
  EXPECT_FALSE(::is_same(0, 0.0));
}
