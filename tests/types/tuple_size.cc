//
//  tuple_size のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/tuple_size.hpp"

#include <utility>
#include <array>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class Tuple>
void check()
{
  static std::size_t const n = std::tuple_size<Tuple>::value;
  
  STATIC_ASSERT((
    std::is_same<
      std::integral_constant<std::size_t, n>, typename etude::tuple_size<Tuple>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_base_of<
      std::integral_constant<std::size_t, n>, etude::tuple_size<Tuple>
    >::value
  ));
  
  STATIC_ASSERT(( etude::tuple_size<Tuple>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple const>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple volatile>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple const volatile>::value == n ));
  
  STATIC_ASSERT(( etude::tuple_size<Tuple &>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple const&>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple &&>::value == n ));
}

template<class NotTuple>
void check_not_tuple()
{
  STATIC_ASSERT((
    std::is_same<
      etude::null_constant<std::size_t>, typename etude::tuple_size<NotTuple>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_base_of<
      etude::null_constant<std::size_t>, etude::tuple_size<NotTuple>
    >::value
  ));
}

int main()
{
  check< std::tuple<> >();
  check< std::tuple<int> >();
  check< std::tuple<int, double, char*> >();
  check< std::pair<double, char*> >();
  check< std::array<int, 0> >();
  check< std::array<int, 5> >();
  
  check_not_tuple<void>();
  check_not_tuple<void*>();
  check_not_tuple<int>();
  check_not_tuple<int&>();
  
  class X{};
  check_not_tuple<X>();
}
