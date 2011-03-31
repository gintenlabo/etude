//
//  tuple_convert のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/tuple_convert.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <type_traits>
#include <utility>
#include <tuple>
#include <array>

int main()
{
  // tuple -> tuple
  {
    int i = 0;
    std::tuple<int, int&, int const&> t( 23, i, i );
    typedef std::tuple<int&, int const&, int> tuple_type;
    
    STATIC_ASSERT((
      std::is_same< decltype( etude::tuple_convert<tuple_type>(t) ), tuple_type >::value
    ));
  }
  
  // tuple -> pair
  {
    int i = 0;
    std::tuple<int, int&> t( 23, i );
    typedef std::pair<int&&, int const&> pair_type;
    
    auto p = etude::tuple_convert<pair_type>( std::move(t) );
    STATIC_ASSERT((
      std::is_same< decltype(p), pair_type >::value
    ));
  }
  
  // pair -> tuple
  {
    int i = 0;
    std::pair<int, int&> p( 23, i );
    typedef std::tuple<int, int&> tuple_type;
    
    STATIC_ASSERT((
      std::is_same< decltype( etude::tuple_convert<tuple_type>(p) ), tuple_type >::value
    ));
    
    auto t = etude::tuple_convert<tuple_type>( std::move(p) );
    STATIC_ASSERT((
      std::is_same< decltype(t), tuple_type >::value
    ));
  }
  
  // array -> tuple
  {
    std::array<int const, 3> a = {{ 1, 2, 3 }};
    typedef std::tuple<int, int const&, unsigned int> tuple_type;
    
    STATIC_ASSERT((
      std::is_same< decltype( etude::tuple_convert<tuple_type>(a) ), tuple_type >::value
    ));
    
    auto t = etude::tuple_convert<tuple_type>(a);
    STATIC_ASSERT((
      std::is_same< decltype(t), tuple_type >::value
    ));
  }
}
