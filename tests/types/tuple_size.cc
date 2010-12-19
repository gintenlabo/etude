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
  
  STATIC_ASSERT(( etude::tuple_size<Tuple>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple const>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple volatile>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple const volatile>::value == n ));
  
  STATIC_ASSERT(( etude::tuple_size<Tuple &>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple const&>::value == n ));
  STATIC_ASSERT(( etude::tuple_size<Tuple &&>::value == n ));
}

int main()
{
  check< std::tuple<> >();
  check< std::tuple<int> >();
  check< std::tuple<int, double, char*> >();
  check< std::pair<double, char*> >();
  check< std::array<int, 0> >();
  check< std::array<int, 5> >();
}
