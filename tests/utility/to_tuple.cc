//
//  to_tuple のテストです。
//    
//  Copyright (C) 2011  Takaya Saito (SubaruG)
//    Distributed under the Boost Software License, Version 1.0.
//    http://www.boost.org/LICENSE_1_0.txt
//

#include "../../etude/utility/to_tuple.hpp"

#define STATIC_ASSERT( expr ) static_assert( expr, #expr )

#include <type_traits>
#include <utility>
#include <tuple>
#include <array>

int main()
{
  {
    int i = 0;
    std::pair<int, int&> p( 23, i );
    typedef std::tuple<int, int&> tuple_type;
    
    STATIC_ASSERT((
      std::is_same< decltype( etude::to_tuple(p) ), tuple_type >::value
    ));
    
    auto t = etude::to_tuple( std::move(p) );
    STATIC_ASSERT((
      std::is_same< decltype(t), tuple_type >::value
    ));
  }
  
  {
    std::array<int const, 3> a = {{ 1, 2, 3 }};
    typedef std::tuple<int const, int const, int const> tuple_type;
    
    STATIC_ASSERT((
      std::is_same< decltype( etude::to_tuple(a) ), tuple_type >::value
    ));
    
    auto t = etude::to_tuple( std::move(a) );
    STATIC_ASSERT((
      std::is_same< decltype(t), tuple_type >::value
    ));
  }
}
