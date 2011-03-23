//
//  tuple_cat のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/tuple_cat.hpp"

#include <boost/test/minimal.hpp>
#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <type_traits>
#include <utility>
#include <tuple>
#include <array>

int test_main( int, char** )
{
  int i = 0;
  std::pair<int, int&> p( 23, i );
  std::array<int const, 3> a = {{ 1, 2, 3 }};
  
  typedef std::tuple<int, int&, int const, int const, int const> tuple_type;
  STATIC_ASSERT((
    std::is_same< decltype( etude::tuple_cat( p, a ) ), tuple_type >::value
  ));
  
  auto t = etude::tuple_cat( std::move(p), std::move(a) );
  STATIC_ASSERT((
    std::is_same< decltype(t), tuple_type >::value
  ));
  
  BOOST_ASSERT(  std::get<0>(t) == 23 );
  BOOST_ASSERT( &std::get<1>(t) == &i );
  BOOST_ASSERT(  std::get<2>(t) == 1 );
  BOOST_ASSERT(  std::get<3>(t) == 2 );
  BOOST_ASSERT(  std::get<4>(t) == 3 );
  
  return 0;
}
