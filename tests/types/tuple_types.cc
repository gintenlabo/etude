//
//  tuple_indices のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/tuple_types.hpp"

#include <utility>
#include <array>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class Tuple, class Types>
void check_types()
{
  STATIC_ASSERT(( std::is_same<
      typename etude::tuple_types<Tuple>::type, Types
    >::value
  ));
  
  STATIC_ASSERT(( std::is_base_of<
      Types, etude::tuple_types<Tuple>
    >::value
  ));
}

template<class Tuple, class... Args>
void check()
{
  check_types< Tuple, etude::types<Args...> >();
  check_types< Tuple const, etude::types<Args const...> >();
  check_types< Tuple volatile, etude::types<Args volatile...> >();
  check_types< Tuple const volatile, etude::types<Args const volatile...> >();
  
  check_types< Tuple &, etude::types<Args &...> >();
  check_types< Tuple const&, etude::types<Args const&...> >();
  check_types< Tuple &&, etude::types<Args &&...> >();
  
}

template<class... Args>
void check_tuple()
{
  check< std::tuple<Args...>, Args... >();
}

int main()
{
  check_tuple<>();
  check_tuple<int>();
  check_tuple<int&, double const&, char*>();
  check< std::pair<double, char*>, double, char* >();
  check< std::array<int, 0> >();
  check< std::array<int, 5>, int, int, int, int, int >();
}
