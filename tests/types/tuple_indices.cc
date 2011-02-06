//
//  tuple_indices のテストです。
//    
//  Copyright (C) 2010  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/types/tuple_indices.hpp"

#include <utility>
#include <array>

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

template<class Tuple, class Indices>
void check_indices()
{
  STATIC_ASSERT(( std::is_same<
      typename etude::tuple_indices<Tuple>::type, Indices
    >::value
  ));
  
  STATIC_ASSERT(( std::is_base_of<
      Indices, etude::tuple_indices<Tuple>
    >::value
  ));
}

template<class Tuple>
void check()
{
  static std::size_t const n = etude::tuple_size<Tuple>::value;
  typedef typename etude::make_indices<n>::type indices_t;
  
  check_indices<Tuple, indices_t>();
  check_indices<Tuple const, indices_t>();
  check_indices<Tuple volatile, indices_t>();
  check_indices<Tuple const volatile, indices_t>();
  
  check_indices<Tuple &, indices_t>();
  check_indices<Tuple const&, indices_t>();
  check_indices<Tuple &&, indices_t>();
  
}

template<class NotTuple>
void check_not_tuple()
{
  STATIC_ASSERT((
    std::is_same<
      etude::void_type, typename etude::tuple_indices<NotTuple>::type
    >::value
  ));
  STATIC_ASSERT((
    std::is_base_of<
      etude::void_type, etude::tuple_indices<NotTuple>
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
